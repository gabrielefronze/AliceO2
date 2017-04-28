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

        bool Rewind();
        bool NextDigit(uint32_t* output);

    private:
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

        bool ApplyMask(short maskIndex, uint32_t &returnValue);
    };

  }

}

#endif //DESERIALIZER_H
