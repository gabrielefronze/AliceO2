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
#include "FairMQDevice.h"
#include "MUONBase/Deserializer.h"
#include "options/FairMQProgOptions.h"
#include "FairMQLogger.h"

using namespace o2::muon::mid;

//_________________________________________________________________________________________________
MIDRatesComputerDevice::MIDRatesComputerDevice() : FairMQDevice()
{
  // The input of this device is the digits stream
  OnData("data-in", &MIDRatesComputerDevice::HandleData);
}

//_________________________________________________________________________________________________
void MIDRatesComputerDevice::InitTask()
{
  LOG(INFO) << "Initializing device";
  fAlgorithm.Init(fConfig->GetValue<std::string>("binmapfile"));
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

  switch (SendRates(fAlgorithm.Output())) {
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
template <typename T>
errMsg MIDRatesComputerDevice::SendRates(std::shared_ptr<std::vector<T>> digitsOut)
{
  DeltaT deltaT(&fChronometerSendData);

  // Message size is kSize T elements for each strip
  uint64_t msgSize = digitsOut.get()->size();

  //    LOG(DEBUG) << "Msgsize is " << msgSize;

  // Instance message as unique pointer
  FairMQMessagePtr msgOut(NewMessage((int)(msgSize * sizeof(T))));

  // Pointer to message payload
  T* dataPointer = reinterpret_cast<T*>(msgOut->GetData());
  size_t iDigit = 0;

  // Copy OutputData in the payload of the message
  for (auto const& itDigits : *(digitsOut.get())) {
    dataPointer[iDigit] = std::move(itDigits);
  }

  // Try to send the message. If unable trigger a error and abort killing the device
  auto status = Send(msgOut, "rates-out");

  //    LOG(DEBUG) << "Send sent " << status << " bits";

  if (status < 0) {
    return kFailedSend;
  }

  //    LOG(DEBUG) << "Rates sent in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";

  return kOk;
}
