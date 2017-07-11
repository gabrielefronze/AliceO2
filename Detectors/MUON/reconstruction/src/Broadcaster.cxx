//
// Created by Gabriele Gaetano Fronzé on 10/07/2017.
//

#include "MUONReconstruction/Broadcaster.h"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
Broadcaster::Broadcaster( bool waiting ){
    fWaiting = waiting;
    FairMQDevice::OnData("input", &Broadcaster::Broadcast);
}

//_________________________________________________________________________________________________
void Broadcaster::InitTask(){

    for ( const auto &chIt : fChannels ){
        for ( const auto &sockIt : chIt.second ){
            std::string sockType = sockIt.GetType();

            if ( sockType == "push" ){
                fOutputChannelNames.emplace_back(chIt.first);
            }
        }
    }

    LOG(DEBUG) << "Broadcaster has been configured with " << fOutputChannelNames.size() << " output channels.";
}

//_________________________________________________________________________________________________
bool Broadcaster::Broadcast( FairMQMessagePtr &msg, int /*index*/ ){
    bool returnValue = true;

    if ( msg->GetSize()<=100 ) return returnValue;

    LOG(DEBUG) << "Sending message..." << msg->GetSize();

    for ( auto const &chNameIt : fOutputChannelNames ){

        FairMQMessagePtr ptr = NewMessage((int)msg->GetSize());
        ptr->Copy(msg);

        LOG(DEBUG) <<  "\tTo channel " << chNameIt;
        returnValue &= (FairMQDevice::Send(ptr, chNameIt) > 0);
    }

    LOG(DEBUG) << "Sent!";

    return ( returnValue || fWaiting ) ;
}