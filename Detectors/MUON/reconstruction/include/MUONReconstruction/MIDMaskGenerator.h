//
// Created by Gabriele Gaetano Fronzé on 23/06/2017.
//

#ifndef O2_DEV_ALO_MIDMASKGENERATOR_H
#define O2_DEV_ALO_MIDMASKGENERATOR_H

#include "FairMQDevice.h"
#include "MUONBase/Enums.h"
#include "MUONBase/OccupancyMapping.h"

namespace AliceO2 {

    namespace MUON {

        class MIDMaskGenerator : public FairMQDevice, public OccupancyMapping{

        public:
            MIDMaskGenerator();
            virtual ~MIDMaskGenerator();

        protected:
            bool HandleData(FairMQMessagePtr&, int);
            virtual void InitTask();

        private:
            struct stripMask{
                UShort_t nDead; // number of elements for deadStripsIDs
                UShort_t nNoisy; // number of elements for noisyStripsIDs
                std::unordered_set<uint32_t> deadStripsIDs; // container of UniqueIDs of dead strips
                std::unordered_set<uint32_t> noisyStripsIDs; // container of UniqueIDs of noisy strips
            };

            stripMask fStructMask;
            stripMask fStructMaskSim;

            void FindNoisy(digitType type);
            void FindDead(digitType type);

            void ResetAll();
            void FillMask();

            bool SendMask();
        };
    }
}


#endif //O2_DEV_ALO_MIDMASKGENERATOR_H
