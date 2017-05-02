//
// Created by Gabriele Gaetano Fronzé on 28/04/2017.
//

#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include "Rtypes.h"
#include "FairMQMessage.h"

namespace AliceO2 {

  namespace MUON {

    class Deserializer {

    public:

        Deserializer();
        Deserializer(FairMQMessagePtr& msg);
        ~Deserializer();

        // Human readable data struct for output
        struct deserializerDataStruct {
            uint32_t fDetElemID;
            uint32_t fBoardID;
            uint32_t fChannel;
            uint32_t fCathode;
        };

        // Iterator like methods
        bool Rewind();
        deserializerDataStruct* NextDigit();
        inline deserializerDataStruct* CurrentDigit(){ return &fOutputDataStruct; }
        inline deserializerDataStruct* operator() (){ return NextDigit(); }

    private:
        // Const values to exclude first 100 bytes of message and perform deserialization
        const uint32_t kHeaderLength = 100;

        uint8_t* fDataPtr;
        uint32_t* fDigitsDataPtr;
        uint32_t fNDigits;
        uint32_t fOffset;
        uint32_t fDigitCounter;
        uint32_t fUniqueID;

        // Internal data container
        deserializerDataStruct fOutputDataStruct;

    };

  }

}

#endif //DESERIALIZER_H
