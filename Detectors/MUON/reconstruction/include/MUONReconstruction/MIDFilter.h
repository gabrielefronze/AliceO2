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

#ifndef MIDFILTER_H
#define MIDFILTER_H

#include "FairMQDevice.h"
#include "MUONBase/Mapping.h"
#include "MUONBase/Enums.h"
#include "MUONBase/Chrono.h"
#include "string.h"
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <dtrace.h>

namespace o2 {

    namespace muon {

        namespace mid {

            class MIDFilter : public FairMQDevice {
            public:
                MIDFilter();

                virtual ~MIDFilter();

            protected:
                bool HandleData(FairMQMessagePtr &, int);

                bool HandleMask(FairMQMessagePtr &, int);

            private:

                struct stripMask {
                    ushort_t nDead; // number of elements for deadStripsIDs
                    ushort_t nNoisy; // number of elements for noisyStripsIDs
                    std::unordered_set<uint32_t> deadStripsIDs; // container of UniqueIDs of dead strips
                    std::unordered_set<uint32_t> noisyStripsIDs; // container of UniqueIDs of noisy strips
                };

                stripMask fMask;

                template<typename T>
                errMsg SendMsg(uint64_t msgSize, T *data);

                //Chrono object to compute duration
                Chrono fChronometer;
            };
        }
    }
}

#endif //MIDFILTER_H