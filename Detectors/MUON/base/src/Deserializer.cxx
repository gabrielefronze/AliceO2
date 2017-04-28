//
// Created by Gabriele Gaetano Fronzé on 28/04/2017.
//

#include "MUONBase/Deserializer.h"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
Deserializer::Deserializer(FairMQMessagePtr& msg):
fDigitCounter(0),
fOffset(1){
    // Internal pointers to data are kept locally
    fDataPtr = reinterpret_cast<uint8_t*>(msg->GetData());
    fDigitsDataPtr = reinterpret_cast<uint32_t*>(fDataPtr + 100);
    fNDigits = fDigitsDataPtr[0];
}

//_________________________________________________________________________________________________
Deserializer::~Deserializer(){
    fDataPtr = 0x0;
    fDigitsDataPtr = 0x0;
}

//_________________________________________________________________________________________________
bool Deserializer::Rewind(){
    // It is enough to reset the digits counter and the offset
    fDigitCounter = 0;
    fOffset = 1;
}

//_________________________________________________________________________________________________
deserializerDataStruct* Deserializer::NextDigit() {

    // Avoid exceding the total number of digits
    if ( fDigitCounter>fNDigits ) return 0x0;

    // Keep track of how much digits have been read
    fDigitCounter++;

    // Control value to detect parsing problems
    bool control = true;

    // Loading data in the data member used by ApplyMask
    fUniqueID = fDigitsDataPtr[fOffset];

    // Apply all the masks defined in the header
    for (short iData = 0; iData < kNumberOfValues ; ++iData) {
        control &= ApplyMask(iData);
    }

    // Go to the following digit leaping unwanted data
    fOffset+=2;

    // If everything is ok return the pointer to the internal dataStruct
    return (control) ? &fOutputDataStruct : 0x0;
}

//_________________________________________________________________________________________________
bool Deserializer::ApplyMask(short maskIndex){
    // Check if the maskIndex is allowed
    if (maskIndex>(kNumberOfValues-1)) return false;

    // Apply the mask
    *(fDataStructItems[maskIndex]) = (fUniqueID & kMasks[maskIndex]) >> kShifts[maskIndex];

    return true;
}
