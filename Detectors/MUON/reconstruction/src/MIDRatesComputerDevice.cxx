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

#include "MUONReconstruction/MIDRatesComputerDevice.h"
#include "options/FairMQProgOptions.h"
#include "MUONBase/Deserializer.h"

using namespace o2::muon::mid;

//_________________________________________________________________________________________________
MIDRatesComputerDevice::MIDRatesComputerDevice() : FairMQDevice::FairMQDevice()
{
  // The input of this device is the digits stream
  FairMQDevice::OnData("data-in", &MIDRatesComputerDevice::HandleData);
}

//_________________________________________________________________________________________________
void MIDRatesComputerDevice::InitTask()
{
  //  LOG(INFO) << "Initializing device";
  fAlgorithm.Init(fConfig->GetValue<std::string>("binmapfile"));
}

errMsg MIDRatesComputerDevice::SendRates(shared_ptr digitsOut)
{
  return kShortMsg;
}

//_________________________________________________________________________________________________
bool MIDRatesComputerDevice::HandleData(FairMQMessagePtr& msg, int /*index*/)
{
  DeltaT deltaT(&fChronometerHandleData);

  // If the message is empty something is going wrong. The process should be aborted.
  if (!msg) {
    //    LOG(ERROR) << "Message pointer not valid, aborting";
    return false;
  }

  // If the input is smaller than the header size the message is empty and we should skip.
  if (msg->GetSize() < 100) {
    //    LOG(ERROR) << "Message empty, skipping";
    return true;
  }

  Deserializer deserializer(msg);

  std::vector<uint32_t> data;
  data.assign(deserializer.GetDataPointer(), deserializer.GetDataPointer() + deserializer.GetNDigits());

  auto returnValue = fAlgorithm.Exec(data);

  return true;
}

//_________________________________________________________________________________________________


//template <typename T>
//errMsg MIDRatesComputerDevice::SendRates()
//{
//  DeltaT deltaT(&fChronometerSendData);
//
//  //  // Message size is kSize T elements for each strip
//  //  uint64_t msgSize = fMapping.fStripVector.size() * digitType::kSize;
//  //
//  //  //    LOG(DEBUG) << "Msgsize is " << msgSize;
//  //
//  //  // Instance message as unique pointer
//  //  FairMQMessagePtr msgOut(NewMessage((int)(msgSize * sizeof(T))));
//  //
//  //  // Pointer to message payload
//  //  T* dataPointer = reinterpret_cast<T*>(msgOut->GetData());
//  //
//  //  // Copy OutputData in the payload of the message
//  //  for (int iData = 0; iData < fMapping.fStripVector.size(); iData++) {
//  //    for (int iType = 0; iType < digitType::kSize; iType++) {
//  //      dataPointer[iData * 3 + iType] = fMapping.fStripVector[iData].digitsCounter[iType];
//  //    }
//  //  }
//  //
//  //  // Try to send the message. If unable trigger a error and abort killing the device
//  //  auto status = Send(msgOut, "rates-out");
//  //
//  //  //    LOG(DEBUG) << "Send sent " << status << " bits";
//  //
//  //  if (status < 0) {
//  //    return kFailedSend;
//  //  }
//  //
//  //  //    LOG(DEBUG) << "Rates sent in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
//
//  return kOk;
//}