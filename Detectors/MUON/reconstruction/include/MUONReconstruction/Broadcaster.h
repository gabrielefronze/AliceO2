//
// Created by Gabriele Gaetano Fronzé on 10/07/2017.
//

#ifndef O2_DEV_ALO_BROADCASTER_H
#define O2_DEV_ALO_BROADCASTER_H

#include "FairMQDevice.h"
#include "FairMQChannel.h"
#include <string>
#include <vector>

namespace AliceO2 {

    namespace MUON {

        class Broadcaster : public FairMQDevice {
        public:
            Broadcaster( bool waiting = true );
            virtual void InitTask();

        private:
            bool Broadcast(FairMQMessagePtr &, int);

            std::vector<std::string> fOutputChannelNames;
            bool fWaiting;
        };
    }

}


#endif //O2_DEV_ALO_BROADCASTER_H
