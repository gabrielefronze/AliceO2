//
// Created by Gabriele Gaetano Fronzé on 28/04/2017.
//

#include "MUONBase/Deserializer.h"

using namespace AliceO2::MUON;

Deserializer::Deserializer():
fDigitCounter(0),
fOffset(1){
    // Internal pointers to data are kept locally
    fDataPtr = 0x0;
    fDigitsDataPtr = 0x0;
    fNDigits = 0x0;
}

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

    return true;
}

//_________________________________________________________________________________________________
Deserializer::deserializerDataStruct* Deserializer::NextDigit() {

    // Keep track of how much digits have been read
    fDigitCounter++;

    // Avoid exceding the total number of digits
    if ( fDigitCounter>fNDigits ) return 0x0;

    // Loading data in the data member used by ApplyMask
    fUniqueID = fDigitsDataPtr[fOffset];

    fData[0] = fDigitsDataPtr[fOffset];
    fData[1] = fDigitsDataPtr[fOffset+1];

    fOutputDataStruct.fDetElemID = fUniqueID & 0xFFF;
    fOutputDataStruct.fBoardID = ( fUniqueID >> 12 ) & 0xFFF;
    fOutputDataStruct.fChannel = ( fUniqueID >> 24 ) & 0x3F;
    fOutputDataStruct.fCathode = ( fUniqueID >> 30 ) & 0x4;


    // Go to the following digit leaping unwanted data
    fOffset+=2;

    // If everything is ok return the pointer to the internal dataStruct
    return &fOutputDataStruct;
}

//_________________________________________________________________________________________________
uint32_t* Deserializer::NextUniqueID() {

    // Keep track of how much digits have been read
    fDigitCounter++;

    // Avoid exceding the total number of digits
    if ( fDigitCounter>fNDigits ) return 0x0;

    // Loading data in the data member used by ApplyMask
    fUniqueID = fDigitsDataPtr[fOffset];

    fData[0] = fDigitsDataPtr[fOffset];
    fData[1] = fDigitsDataPtr[fOffset+1];

    // Go to the following digit leaping unwanted data
    fOffset+=2;

    // If everything is ok return the pointer to the internal dataStruct
    return &fUniqueID;
}
