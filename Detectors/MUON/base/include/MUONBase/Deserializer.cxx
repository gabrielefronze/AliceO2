//
// Created by Gabriele Gaetano Fronzé on 28/04/2017.
//

#include "Deserializer.h"

using namespace AliceO2::MUON;

Deserializer::Deserializer(FairMQMessagePtr& msg):
fDigitCounter(0),
fOffset(1){
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
    fOffset = 1;
}

dataStruct* Deserializer::NextDigit() {

    if ( fDigitCounter>fNDigits ) return 0x0;

    fDigitCounter++;
    bool control = true;
    fUniqueID = fDigitsDataPtr[fOffset++];

    for (short iData = 0; iData < kNumberOfValues ; ++iData) {
        control &= ApplyMask(iData);
        fOffset++;
    }

    return (control) ? &fOutputDataStruct : 0x0;
}

bool Deserializer::ApplyMask(short maskIndex){
    if (maskIndex>(kNumberOfValues-1)) return false;

    *(fDataStructItems[maskIndex]) = (fUniqueID & kMasks[maskIndex]) >> kShifts[maskIndex];

    return true;
}
