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

        class MIDFilter : public FairMQDevice
        {
        public:
            MIDFilter();
            virtual ~MIDFilter();

        protected:
            bool HandleData(FairMQMessagePtr&, int);
            virtual void InitTask();
        };
    }
}

#endif //MIDFILTER_H