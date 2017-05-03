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
                uint64_t startTS, stopTS; // timestamps
                uint64_t digitsCounter; // counter of time the strip has been fired
                UChar_t nNeighbours; // number of neighboursUniqueID
                Long64_t neighboursUniqueID[10]; // indices of neighboursUniqueID in array stored in mpDE
                Float_t area[2][2]; // 2D area
            };

            std::string fMapFilename;
            std::unordered_map<uint64_t,stripMapping> fInternalMapping;

            bool ReadMapping ( const char* );

        };

    }

}

#endif //MIDOCCUPANCY_H
