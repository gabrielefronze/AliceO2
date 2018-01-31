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

#include "MUONReconstruction/MIDFilterDevice.h"
#include "FairMQLogger.h"
#include "MUONBase/Chrono.h"
#include "MUONBase/Deserializer.h"
#include "options/FairMQProgOptions.h"

using namespace o2::muon::mid;

//_________________________________________________________________________________________________
MIDFilterDevice::MIDFilterDevice()
{
  FairMQDevice::OnData("mask-in", &MIDFilterDevice::HandleMask);
  FairMQDevice::OnData("digits-in", &MIDFilterDevice::HandleData);
}

//_________________________________________________________________________________________________
MIDFilterDevice::~MIDFilterDevice() {
  LOG(INFO) << "Average filtering performance: " << fChronometerMasking.PrintStatus();
  LOG(INFO) << "Average mask loading performance: " << fChronometerLoading.PrintStatus();
}

//_________________________________________________________________________________________________
bool MIDFilterDevice::HandleData(FairMQMessagePtr& msg, int /*index*/)
{
  DeltaT deltaT(&fChronometerMasking);

  if (!msg) {
    LOG(ERROR) << "Message pointer not valid, aborting";
    return false;
  }

  if (msg->GetSize() < 100) {
    //        LOG(ERROR) << "Message empty, skipping";
    return true;
  }

  // Deserializer will simplify the reading of the input message
  Deserializer MessageDeserializer(msg);

  // This vector will contain the data and will be passed to the filtering algorithm
  std::vector<uint32_t> data;
  data.assign(MessageDeserializer.GetDataPointer(),
              MessageDeserializer.GetDataPointer() + MessageDeserializer.GetNDigits());

  // returnValue is false if no masking has been done
  auto returnValue = fAlgorithm.ExecFilter(data);

  if (returnValue) {
    data.erase(std::remove_if(data.begin(), data.end(), [](uint32_t UID) { return UID == 0; }), data.end());
  }

  switch (SendMsg(data.size(), &data[0])) {
    case kShortMsg:
      LOG(ERROR) << "Message shorter than expected. Skipping.";
      return true;

    case kFailedSend:
      LOG(ERROR) << "Problems sending masked digits. Aborting.";
      return false;

    case kOk:
      return returnValue;

    default:
      return returnValue;
  }
}

//_________________________________________________________________________________________________
bool MIDFilterDevice::HandleMask(FairMQMessagePtr& msg, int /*index*/)
{
  DeltaT deltaT(&fChronometerLoading);

  // The header is made of two ushort which are counters of the
  // number of dead and noisy strips, respectively
  unsigned short* maskHeader = reinterpret_cast<unsigned short*>(msg->GetData());

  // Load unique IDs in maskData
  uint32_t* maskData = reinterpret_cast<uint32_t*>(&(maskHeader[2]));

  return fAlgorithm.ExecMaskLoading(maskHeader, maskData);
}

//_________________________________________________________________________________________________
template <typename T>
errMsg MIDFilterDevice::SendMsg(uint64_t msgSize, T* data)
{
  // Create unique pointer to a message of the right size
  FairMQMessagePtr msgOut(NewMessage((int)(msgSize * sizeof(T))));

  // Cast the pointer to the message payload to std::vector pointer to simplify copy
  T* dataPointer = reinterpret_cast<T*>(msgOut->GetData());

  int counter = 0;

  // Copy OutputData in the payload of the message
  for (int iData = 0; iData < msgSize; iData++) {
    dataPointer[iData] = data[iData];
    counter++;
  }

  // Just a check that will be deleted after some testing
  if (counter < msgSize - 1) {
    return kShortMsg;
  }

  //    LOG(INFO) << "Sending masked message containing " << counter << " digits";

  // Try to send the message. If unable trigger a error and abort killing the device
  auto returnValue = SendAsync(msgOut, "digits-out");

  if (returnValue < 0) {
    LOG(ERROR) << "Send failed with error code: " << returnValue;
    return kFailedSend;
  }

  return kOk;
}