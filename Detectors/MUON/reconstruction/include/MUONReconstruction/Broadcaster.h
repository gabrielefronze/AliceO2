#ifndef O2_DEV_ALO_BROADCASTER_H
#define O2_DEV_ALO_BROADCASTER_H

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
///  @file   Broadcaster
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Device to perform BCast of the
///

#include "FairMQDevice.h"
#include "FairMQChannel.h"
#include "MUONBase/Chrono.h"
#include <string>
#include <vector>

namespace o2 {

    namespace muon {

        namespace mid {

            class Broadcaster : public FairMQDevice {
            public:
                Broadcaster(bool waiting = true);

                ~Broadcaster() {
                    LOG(INFO) << "Average performance: " << fChronometer.PrintStatus();
                };

                virtual void InitTask();

            private:
                bool Broadcast(FairMQMessagePtr &, int);

                std::vector<std::string> fOutputChannelNames;
                bool fWaiting;

                Chrono fChronometer;
            };
        }
    }
}


#endif //O2_DEV_ALO_BROADCASTER_H
