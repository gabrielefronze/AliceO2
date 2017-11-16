//
// Created by Gabriele Gaetano Fronzé on 28/04/2017.
//

#include "MUONBase/Deserializer.h"

using namespace AliceO2::MUON;

Deserializer::Deserializer():
fDigitCounter(0),
fOffset(1){
    // Internal pointers to data are kept locally
    fDataPtr = nullptr;
    fDigitsDataPtr = nullptr;
    fNDigits = nullptr;
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
    fDataPtr = nullptr;
    fDigitsDataPtr = nullptr;
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

    if (!Advance()) return nullptr;
    Load();

    // If everything is ok return the pointer to the internal dataStruct
    return &fOutputDataStruct;
}

//_________________________________________________________________________________________________
uint32_t* Deserializer::NextUniqueID(bool loadAllData) {

    if (!Advance()) return nullptr;
    if (loadAllData) Load();

    // If everything is ok return the pointer to the internal dataStruct
    return &fUniqueID;
}

//_________________________________________________________________________________________________
bool Deserializer::Advance() {
    // Keep track of how much digits have been read
    fDigitCounter++;

    // Avoid exceding the total number of digits
    if ( fDigitCounter>fNDigits ) return false;

    // Loading data in the data member used by ApplyMask
    fUniqueID = fDigitsDataPtr[fOffset];

    fData[0] = fDigitsDataPtr[fOffset];
    fData[1] = fDigitsDataPtr[fOffset+1];

    // Go to the following digit leaping unwanted data
    fOffset+=2;

    return true;
}

//_________________________________________________________________________________________________
void Deserializer::Load() {
    fOutputDataStruct.fDetElemID = fUniqueID & 0xFFF;
    fOutputDataStruct.fBoardID = ( fUniqueID >> 12 ) & 0xFFF;
    fOutputDataStruct.fChannel = ( fUniqueID >> 24 ) & 0x3F;
    fOutputDataStruct.fCathode = ( fUniqueID >> 30 ) & 0x4;
}
