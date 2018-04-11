//
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @author  Gabriele Gaetano Fronzé

#include "MIDMaskGeneratorDevice.h"
#include "options/FairMQProgOptions.h"

namespace o2
{
namespace mid
{

//_________________________________________________________________________________________________
MIDMaskGeneratorDevice::MIDMaskGeneratorDevice() : FairMQDevice()
{
  OnData("rates-in", &MIDMaskGeneratorDevice::HandleData);
}

//_________________________________________________________________________________________________
MIDMaskGeneratorDevice::~MIDMaskGeneratorDevice()
{
  // TODO: the destructor should use the Output method
  //  LOG(DEBUG) << "Detected noisy strips:";
  //  for (const auto& itMask : fStructMask.noisyStripsIDs) {
  //    LOG(DEBUG) << "\t" << itMask << "\t\t" << fMapping[itMask]->digitsCounter[digitType::kPhysics];
  //  }
}

//_________________________________________________________________________________________________
void MIDMaskGeneratorDevice::InitTask()
{
  LOG(INFO) << "Initializing device";

  // Loading mapping at startup
  std::string mapFilename = fConfig->GetValue<std::string>("binmapfile");
  if (!(fAlgorithm.Init(mapFilename.c_str()))) {
    LOG(ERROR) << "Error reading the mapping from " << mapFilename;
  } else {
    LOG(INFO) << "Mapping correctly loaded.";
  }
}

//_________________________________________________________________________________________________
bool MIDMaskGeneratorDevice::HandleData(FairMQMessagePtr& msg, int /*index*/)
{
  // If the message is empty something is going wrong. The process should be aborted.
  if (!msg) {
    LOG(ERROR) << "Message pointer not valid, aborting";
    return false;
  }

  // If the input is smaller than the header size the message is empty and we should skip.
  if (msg->GetSize() < 1) {
    LOG(ERROR) << "Message empty, skipping";
    return true;
  }

  LOG(DEBUG) << "Received valid message";

  uint64_t* dataPointer = reinterpret_cast<uint64_t*>(msg->GetData());

  auto returnValue = fAlgorithm.Exec(dataPointer);

  returnValue &= (SendMask() == errMsg::kOk);

  return returnValue;
}

//_________________________________________________________________________________________________
errMsg MIDMaskGeneratorDevice::SendMask()
{
  auto mask = fAlgorithm.GetMask();

  auto sum = mask->nDead + mask->nNoisy;

  if (sum == 0)
    return kOk;

  int msgSize = sizeof(mask->nDead) + sizeof(mask->nNoisy) + sum * sizeof(uint32_t);
  FairMQMessagePtr msgOut(NewMessage(msgSize));

  auto header = reinterpret_cast<ushort_t*>(msgOut->GetData());
  header[0] = mask->nDead;
  header[1] = mask->nNoisy;
  auto payload = reinterpret_cast<uint32_t*>(&(header[2]));

  int position = 0;

  for (auto const& itDead : mask->deadStripsIDs) {
    payload[position++] = itDead;
  }
  for (auto const& itNoisy : mask->noisyStripsIDs) {
    payload[position++] = itNoisy;
  }

  // Try to send the message. If unable trigger a error and abort killing the device
  if (SendAsync(msgOut, "mask-out") < 0) {
    return kFailedSend;
  }

  return kOk;
}

} // namespace mid
} // namespace o2
