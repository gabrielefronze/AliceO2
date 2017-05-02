//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#ifndef MIDOCCUPANCY_H
#define MIDOCCUPANCY_H

#include "Rtypes.h"
#include "FairMQDevice.h"
#include "MUONBase/Deserializer.h"
#include "MUONBase/Mapping.h"
#include "TString.h"

namespace AliceO2 {

    namespace MUON {

        class MIDoccupancy : public FairMQDevice
        {
        public:
            MIDoccupancy(TString mapFilename);
            virtual ~MIDoccupancy();

        protected:
            bool HandleData(FairMQMessagePtr&, int);
            virtual void InitTask();

        private:
            Mapping::mpDE* fInternalMapping;
            Deserializer fMessageDeserializer;
            TString fMapFilename;

        };

    }

}

#endif //MIDOCCUPANCY_H
