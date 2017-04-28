//
// Created by Gabriele Gaetano Fronzé on 28/04/2017.
//

#include "Deserializer.h"

using namespace AliceO2::MUON;

Deserializer::Deserializer(FairMQMessagePtr& msg):
fDigitCounter(0),
fOffset(uint32_t(1)){
    fDataPtr = reinterpret_cast<uint8_t*>(msg->GetData());
    fDigitsDataPtr = reinterpret_cast<uint32_t*>(fDataPtr + 100);
    fNDigits = fDigitsDataPtr[0];
}

Deserializer::~Deserializer(){
    fDataPtr = 0x0;
    fDigitsDataPtr = 0x0;
}

bool Deserializer::Rewind(){
    fDigitCounter = 0;
    fOffset = uint32_t(1);
}

bool Deserializer::NextDigit(uint32_t *output) {

    if ( fDigitCounter>fNDigits ) return false;

    fDigitCounter++;
    bool control = true;
    uint32_t outputArray[kNumberOfValues];
    fUniqueID = fDigitsDataPtr[fOffset++];

    for (short iData = 0; iData < kNumberOfValues ; ++iData) {
        control &= ApplyMask(iData,outputArray[iData]);
        fOffset++;
    }

    output = outputArray;

    return control;
}

bool Deserializer::ApplyMask(short maskIndex, uint32_t &returnValue){
    if (maskIndex>(kNumberOfValues-1)) return false;

    returnValue = (fUniqueID & kMasks[maskIndex]) >> kShifts[maskIndex];

    return true;
}
