//
// Created by Gabriele Gaetano Fronzé on 28/04/2017.
//

#include "MUONBase/Deserializer.h"
#include "FairMQMessage.h"


AliceO2::MUON::kMasks = {0xFFF,0xFFF000,0x3F000000,0x40000000};
AliceO2::MUON::kShifts  = {0,12,24,30};

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
Deserializer::Deserializer(FairMQMessagePtr& msg):
fDigitCounter(0),
fOffset(1){
    // Internal pointers to data are kept locally
    fDataPtr = reinterpret_cast<uint8_t*>(msg->GetData());
    fDigitsDataPtr = reinterpret_cast<uint32_t*>(fDataPtr + kHeaderLength);
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

    fOutputDataStruct.fDetElemID = fUniqueID & 0xFFF;
    fOutputDataStruct.fBoardID = ( fUniqueID >> 12 ) & 0xFFF;
    fOutputDataStruct.fChannel = ( fUniqueID >> 24 ) & 0x3F;
    fOutputDataStruct.fCathode = ( fUniqueID >> 30 ) & 0x4;


    // Go to the following digit leaping unwanted data
    fOffset+=2;

    // If everything is ok return the pointer to the internal dataStruct
    return (control) ? &fOutputDataStruct : 0x0;
}
