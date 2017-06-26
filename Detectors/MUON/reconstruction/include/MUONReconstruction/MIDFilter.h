//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#ifndef MIDFILTER_H
#define MIDFILTER_H

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

        class MIDFilter : public FairMQDevice {
        public:
            MIDFilter();

            virtual ~MIDFilter();

        protected:
            bool HandleData(FairMQMessagePtr &, int);

            bool HandleMask(FairMQMessagePtr &, int);

            virtual void InitTask();

        private:

            struct stripMask {
                UShort_t nDead; // number of elements for deadStripsIDs
                UShort_t nNoisy; // number of elements for noisyStripsIDs
                std::unordered_set<uint32_t> deadStripsIDs; // container of UniqueIDs of dead strips
                std::unordered_set<uint32_t> noisyStripsIDs; // container of UniqueIDs of noisy strips
            };

            stripMask fMask;

            enum errMsg{
                kShortMsg,
                kFailedSend,
                kOk
            };

            template<typename T> errMsg SendMsg(uint64_t msgSize, T* data);
        };
    }
}

#endif //MIDFILTER_H