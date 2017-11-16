//
// Created by Gabriele Gaetano Fronzé on 26/06/2017.
//

#ifndef O2_DEV_ALO_OCCUPANCYMAPPING_H
#define O2_DEV_ALO_OCCUPANCYMAPPING_H

#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sys/dtrace.h>
#include "MUONBase/Enums.h"

namespace AliceO2 {

    namespace MUON {

        struct stripMapping {
            uint64_t startTS[digitType::kSize]; // timestamp of first added run
            uint64_t stopTS[digitType::kSize]; // timestamp of last added run
            uint64_t digitsCounter[digitType::kSize]; // counter of time the strip has been fired
            ushort_t columnID;
            float_t area; // 1D area
            float_t coord[2][2];
            float_t rate[digitType::kSize]; // rate in Hz/cm2 or 1/cm2
            bool isDead;
            bool isNoisy;
            stripMapping();
        };

        class OccupancyMapping {

        public:
            bool ReadMapping(const char*,int elementID);
            bool ReadMapping(const char*,std::vector<int> elementIDs);
            bool ReadMapping(const char*);

            stripMapping* operator[](uint32_t ID){
                auto stripFinder = fIDMap.find(ID);
                if (stripFinder == fIDMap.end()){
                    return nullptr;
                }
                return &(fStripVector[stripFinder->second]);
            }

            bool Consistent(bool deep = false){

                bool counterStatus = (fIDMap.size() == fStripVector.size());

                if(deep && counterStatus){
                    for(const auto &it : fStripVector){
                        for (const auto &iCounter : it.digitsCounter) {
                            counterStatus = counterStatus && iCounter;
                        }
                    }
                }

                return counterStatus;
            };

            std::unordered_map<uint32_t, size_t> fIDMap;
            std::vector<stripMapping> fStripVector;
            std::vector<stripMapping *> fStructsBuffer;
        };
    }
}


#endif //O2_DEV_ALO_OCCUPANCYMAPPING_H
