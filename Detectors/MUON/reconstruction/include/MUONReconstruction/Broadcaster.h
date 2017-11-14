#ifndef O2_DEV_ALO_BROADCASTER_H
#define O2_DEV_ALO_BROADCASTER_H

///
///  @file   Broadcaster
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Device to perform BCast of the
///

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
