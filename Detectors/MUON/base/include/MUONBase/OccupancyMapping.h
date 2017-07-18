//
// Created by Gabriele Gaetano Fronzé on 26/06/2017.
//

#ifndef O2_DEV_ALO_OCCUPANCYMAPPING_H
#define O2_DEV_ALO_OCCUPANCYMAPPING_H

#include "Rtypes.h"
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "MUONBase/Enums.h"

namespace AliceO2 {

    namespace MUON {

        struct stripMapping {
            uint64_t startTS[kSize]; // timestamp of first added run
            uint64_t stopTS[kSize]; // timestamp of last added run
            uint64_t digitsCounter[kSize]; // counter of time the strip has been fired
            UShort_t columnID;
            Float_t area; // 1D area
            Float_t coord[2][2];
            Float_t rate[kSize]; // rate in Hz/cm2 or 1/cm2
            Bool_t isDead;
            Bool_t isNoisy;
        };

        class OccupancyMapping {

        public:
            bool ReadMapping(const char*);

            std::unordered_map<uint32_t, stripMapping*> fIDMap;
            std::vector<stripMapping> fStripVector;
            std::vector<stripMapping *> fStructsBuffer;
        };
    }
}


#endif //O2_DEV_ALO_OCCUPANCYMAPPING_H
