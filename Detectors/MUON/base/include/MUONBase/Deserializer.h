//
// Created by Gabriele Gaetano Fronzé on 28/04/2017.
//

#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include "FairMQMessage.h"

namespace AliceO2 {

  namespace MUON {

    class Deserializer {

    public:

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
        const UInt_t kHeaderLength = 100;
        const short kNumberOfValues = 4;
        static const UInt_t kMasks[kNumberOfValues] = {0xFFF,0xFFF000,0x3F000000,0x40000000};
        static const UInt_t kShifts[kNumberOfValues] = {0,12,24,30};


        uint8_t* fDataPtr;
        uint32_t* fDigitsDataPtr;
        uint32_t fNDigits;
        uint32_t fOffset;
        uint32_t fDigitCounter;
        uint32_t fUniqueID;

        // Internal data container and array of pointers (neede in ApplyMask)
        deserializerDataStruct fOutputDataStruct;
        uint32_t* fDataStructItems[kNumberOfValues] = {&(fOutputDataStruct.fDetElemID),&(fOutputDataStruct.fBoardID),&(fOutputDataStruct.fChannel),&(fOutputDataStruct.fCathode)};

        // Method to extract data using the masks and shifts defined above
        bool ApplyMask(short maskIndex);
    };

  }

}

#endif //DESERIALIZER_H
