//
// Created by Gabriele Gaetano Fronzé on 26/06/2017.
//

#ifndef O2_DEV_ALO_OCCUPANCYMAPPING_H
#define O2_DEV_ALO_OCCUPANCYMAPPING_H

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
            short columnID;
            float area; // 1D area
            float coord[2][2];
            float rate[kSize]; // rate in Hz/cm2 or 1/cm2
            bool isDead;
            bool isNoisy;
        };

        class OccupancyMapping {

        public:
            bool ReadMapping(const char*,int elementID);
            bool ReadMapping(const char*,std::vector<int> elementIDs);
            bool ReadMapping(const char*);

            std::unordered_map<uint32_t, stripMapping*> fIDMap;
            std::vector<stripMapping> fStripVector;
            std::vector<stripMapping *> fStructsBuffer;
        };
    }
}


#endif //O2_DEV_ALO_OCCUPANCYMAPPING_H
