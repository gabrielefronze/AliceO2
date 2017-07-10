//
// Created by Gabriele Gaetano Fronzé on 10/07/2017.
//

#ifndef O2_DEV_ALO_BROADCASTER_H
#define O2_DEV_ALO_BROADCASTER_H

#include "FairMQDevice.h"
#include "FairMQChannel.h"
#include <string>
#include <future>
#include <deque>
#include <vector>
#include <thread>

namespace AliceO2 {

    namespace MUON {

        class Broadcaster : public FairMQDevice {
        public:
            Broadcaster( bool waiting = true );
            virtual ~Broadcaster();

        protected:
            bool Broadcast(FairMQMessagePtr &, int);
            bool BroadSend(FairMQMessagePtr &msg_ptr, std::string channelName);

        private:
            std::vector<std::string> fOutputChannelNames;
            std::deque<std::future<bool>> fSendQueue;
            bool fWaiting;
        };
    }

}


#endif //O2_DEV_ALO_BROADCASTER_H
