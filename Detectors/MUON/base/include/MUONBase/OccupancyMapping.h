//
// Created by Gabriele Gaetano Fronzé on 26/06/2017.
//

#ifndef O2_DEV_ALO_OCCUPANCYMAPPING_H
#define O2_DEV_ALO_OCCUPANCYMAPPING_H

#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace AliceO2 {

    namespace MUON {

        enum digitType {
            kPhysics,
            kFET,
            kTriggered,
            kSize
        };

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
        protected:
            std::unordered_map<uint32_t, stripMapping *> fInternalMapping;
            std::array<stripMapping, 20992> fStripVector;
            std::vector<stripMapping *> fStructsBuffer;

            bool ReadMapping(const char*);
        };
    }
}


#endif //O2_DEV_ALO_OCCUPANCYMAPPING_H
