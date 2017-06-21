//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#ifndef MIDOCCUPANCY_H
#define MIDOCCUPANCY_H

#include "Rtypes.h"
#include "FairMQDevice.h"
#include "MUONBase/Mapping.h"
#include "string.h"
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "TPave.h"


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
                UShort_t columnID;
                Float_t area; // 1D area
                Float_t coord[2][2];
                Float_t rate; // rate in Hz/cm2 or 1/cm2
                Bool_t isDead;
                Bool_t isNoisy;
            };

            struct stripMask{
                UShort_t nDead; // number of elements for deadStripsIDs
                UShort_t nNoisy; // number of elements for noisyStripsIDs
                std::unordered_set<uint32_t> deadStripsIDs; // container of UniqueIDs of dead strips
                std::unordered_set<uint32_t> noisyStripsIDs; // container of UniqueIDs of noisy strips
            };

            std::unordered_map<uint32_t,stripMapping*> fInternalMapping;
            std::array<stripMapping,20992> fStripVector;
            std::vector<stripMapping*> fStructsBuffer;
            stripMask fStructMask;
            stripMask fStructMaskSim;

            bool ReadMapping(const char*);

            void ResetCounters(uint64_t newStartTS);
            bool EnoughStatistics();

            void ComputeRate(stripMapping* strip);
            void ComputeAllRates();
            
            bool SendMask();
            int InitMonitor();
            int SendMonitorData();

        };

    }

}

#endif //MIDOCCUPANCY_H
