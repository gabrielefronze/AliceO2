//
// Created by Gabriele Gaetano Fronzé on 26/06/2017.
//

#ifndef O2_DEV_ALO_MIDRATESCOMPUTER_H
#define O2_DEV_ALO_MIDRATESCOMPUTER_H

#include "FairMQDevice.h"
#include "MUONBase/Enums.h"
#include "MUONBase/OccupancyMapping.h"
#include "MUONBase/Mapping.h"
#include "string.h"
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "TPave.h"


namespace AliceO2 {

    namespace MUON {

        class MIDRatesComputer : public FairMQDevice{
        public:
            MIDRatesComputer();
            virtual ~MIDRatesComputer();

        protected:
            bool HandleData(FairMQMessagePtr&, int);
            virtual void InitTask();

        private:

            OccupancyMapping fMapping;

            struct stripMask{
                short nDead; // number of elements for deadStripsIDs
                short nNoisy; // number of elements for noisyStripsIDs
                std::unordered_set<uint32_t> deadStripsIDs; // container of UniqueIDs of dead strips
                std::unordered_set<uint32_t> noisyStripsIDs; // container of UniqueIDs of noisy strips
            };

            stripMask fStructMaskSim;

//            bool ReadMapping(const char*);

            void ResetCounters(uint64_t newStartTS, digitType type);
            bool EnoughStatistics(digitType type);

            void ComputeRate(stripMapping* strip);
            void ComputeAllRates();

            template<typename T> errMsg SendRates();
        };

    }

}


#endif //O2_DEV_ALO_MIDRATESCOMPUTER_H
