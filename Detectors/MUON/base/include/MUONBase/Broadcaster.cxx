//
// Created by Gabriele Gaetano Fronzé on 10/07/2017.
//

#include "Broadcaster.h"
#include <boost/range/adaptor/reversed.hpp>

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
Broadcaster::Broadcaster( bool waiting ){

    fWaiting = waiting;

    for ( const auto &chIt : fChannels ){
        for ( const auto &sockIt : chIt.second ){
            std::string sockType = sockIt.GetType();

            if ( sockType == "push" ){
                fOutputChannelNames.emplace_back(chIt.first);
            }
        }
    }

    LOG(DEBUG) << "Broadcaster has been configured with " << fOutputChannelNames.size() << " output channels.";

    FairMQDevice::OnData("input", &Broadcaster::Broadcast);
}

//_________________________________________________________________________________________________
bool Broadcaster::Broadcast( FairMQMessagePtr &msg, int /*index*/ ){
    for ( auto const &chNameIt : fOutputChannelNames ){
        fSendQueue.emplace_front(std::async(Broadcaster::Send,msg,chNameIt));
    }

    if ( fWaiting ){

        int Pos = 0;

        for (auto &futIt : fSendQueue) {
            futIt.wait();
            fSendQueue.erase(fSendQueue.begin() + Pos);
            Pos++;
        }
    }
}

//_________________________________________________________________________________________________
bool Broadcaster::BroadSend( FairMQMessagePtr &msg, std::string channelName ){
    FairMQDevice::Send(msg, channelName);
}