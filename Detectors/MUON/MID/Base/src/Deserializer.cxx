//
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @author  Gabriele Gaetano Fronzé

#include "MIDBase/Deserializer.h"

using namespace o2::mid;

Deserializer::Deserializer() : fDigitCounter(0), fOffset(1)
{
  // Internal pointers to data are kept locally
  fDataPtr = nullptr;
  fDigitsDataPtr = nullptr;
  fNDigits = 0;
}

//_________________________________________________________________________________________________
Deserializer::Deserializer(FairMQMessagePtr& msg) : Deserializer(msg->GetData()) {}

//_________________________________________________________________________________________________
Deserializer::Deserializer(void* payload) : fDigitCounter(0), fOffset(1)
{
  // Internal pointers to data are kept locally
  fDataPtr = reinterpret_cast<uint32_t*>(payload);

  //    for (uint32_t iHeader = 0; iHeader < kHeaderLength; ++iHeader) {
  //        std::cout<<fDataPtr[iHeader]<<std::endl;
  //    }

  fDigitsDataPtr = fDataPtr + kHeaderLength;
  fNDigits = fDigitsDataPtr[0];
}

//_________________________________________________________________________________________________
Deserializer::~Deserializer()
{
  fDataPtr = nullptr;
  fDigitsDataPtr = nullptr;
}

//_________________________________________________________________________________________________
bool Deserializer::Rewind()
{
  // It is enough to reset the digits counter and the offset
  fDigitCounter = 0;
  fOffset = 1;

  return true;
}

//_________________________________________________________________________________________________
deserializerDataStruct* Deserializer::NextDigit()
{
  if (!Advance())
    return nullptr;
  Load();

  // If everything is ok return the pointer to the internal dataStruct
  return &fOutputDataStruct;
}

//_________________________________________________________________________________________________
uint32_t* Deserializer::NextUniqueID(bool loadAllData)
{
  if (!Advance())
    return nullptr;
  if (loadAllData)
    Load();

  // If everything is ok return the pointer to the internal dataStruct
  return &fUniqueID;
}

//_________________________________________________________________________________________________
bool Deserializer::Advance()
{
  // Keep track of how much digits have been read
  fDigitCounter++;

  // Avoid exceding the total number of digits
  if (fDigitCounter > fNDigits)
    return false;

  // Loading UID and Data
  fUniqueID = fDigitsDataPtr[fOffset];

  fData[0] = fDigitsDataPtr[fOffset];
  fData[1] = fDigitsDataPtr[fOffset + 1];

  // Go to the following digit leaping unwanted data
  fOffset += 2;

  return true;
}

//_________________________________________________________________________________________________
void Deserializer::Load()
{
  fOutputDataStruct.fDetElemID = fUniqueID & 0xFFF;
  fOutputDataStruct.fBoardID = (fUniqueID >> 12) & 0xFFF;
  fOutputDataStruct.fChannel = (fUniqueID >> 24) & 0x3F;
  fOutputDataStruct.fCathode = (fUniqueID >> 30) & 0x4;
}
