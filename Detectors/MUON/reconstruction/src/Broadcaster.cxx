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

#include "MUONReconstruction/Broadcaster.h"

using namespace o2::muon::mid;

//_________________________________________________________________________________________________
Broadcaster::Broadcaster(bool waiting)
{
  fWaiting = waiting;
  FairMQDevice::OnData("input", &Broadcaster::Broadcast);
}

//_________________________________________________________________________________________________
void Broadcaster::InitTask()
{
  for (const auto& chIt : fChannels) {
    for (const auto& sockIt : chIt.second) {
      std::string sockType = sockIt.GetType();

      if (sockType == "push") {
        fOutputChannelNames.emplace_back(chIt.first);
      }
    }
  }

  LOG(DEBUG) << "Broadcaster has been configured with " << fOutputChannelNames.size() << " output channels.";
}

//_________________________________________________________________________________________________
bool Broadcaster::Broadcast(FairMQMessagePtr& msg, int /*index*/)
{
  DeltaT deltaT(&fChronometer);

  bool returnValue = true;

  //    if ( msg->GetSize()<=100 ) return returnValue;

  //    LOG(DEBUG) << "Sending message..." << msg->GetSize();

  for (auto const& chNameIt : fOutputChannelNames) {
    FairMQMessagePtr ptr = NewMessage((int)msg->GetSize());
    ptr->Copy(msg);

    //        LOG(DEBUG) <<  "\tTo channel " << chNameIt;
    returnValue &= (FairMQDevice::Send(ptr, chNameIt) > 0);
  }

  //    LOG(DEBUG) << "Sent!";

  return (returnValue || fWaiting);
}