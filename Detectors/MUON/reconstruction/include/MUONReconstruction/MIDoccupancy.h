//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#ifndef MIDOCCUPANCY_H
#define MIDOCCUPANCY_H

#include "Rtypes.h"
#include "FairMQDevice.h"
#include "MUONBase/Mapping.h"
#include "string.h"

namespace AliceO2 {

    namespace MUON {

        class MIDoccupancy : public FairMQDevice
        {
        public:
            MIDoccupancy();
            virtual ~MIDoccupancy();

        protected:
            bool HandleData(FairMQMessagePtr&, int);
            virtual void InitTask();

        private:

            struct stripMapping{
                uint64_t startTS; // timestamp of first added run
                uint64_t stopTS; // timestamp of last added run
                uint64_t digitsCounter; // counter of time the strip has been fired
                UChar_t nNeighbours; // number of neighbours
                uint64_t neighboursUniqueIDs[10]; // uniqueIDs of neighbours
                Float_t area; // 1D area
                Float_t rate; // rate in Hz/cm2 or 1/cm2
                Float_t neighboursRate;
                Bool_t isDead;
                Bool_t isNoisy;
                Bool_t useMe;
            };

            std::string fMapFilename;
            std::unordered_map<uint64_t,stripMapping> fInternalMapping;

            bool ReadMapping(const char*);

            void ResetUseMe(Bool_t value = true);
            void ResetCounters(uint64_t newStartTS);

            void ComputeRate(stripMapping* strip);
            void ComputeAllRates();

            double GetMeanRate(stripMapping* strip, uint depth=1);
            double RecursiveGetRateSum(stripMapping* strip, uint &counter, uint depth=1);
            void ComputeIsDead(stripMapping* strip);
            void ComputeAllIsDead();
            void ComputeIsNoisy(stripMapping* strip);
            void ComputeAllIsNoisy();

        };

    }

}

#endif //MIDOCCUPANCY_H
