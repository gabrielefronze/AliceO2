#ifndef DESERIALIZER_H
#define DESERIALIZER_H

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
///  @file   Deserializer
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Little deserializer app to unpack MID messages
///

#include "DataStructs.h"
#include "FairMQMessage.h"

namespace o2
{
namespace muon
{
namespace mid
{
class Deserializßer
{
 public:
  Deserializer();

  explicit Deserializer(FairMQMessagePtr& msg);

  explicit Deserializer(void* payload);

  ~Deserializer();

  // Iterator like methods
  bool Rewind();

  deserializerDataStruct* NextDigit();

  inline deserializerDataStruct* CurrentDigit() { return &fOutputDataStruct; }

  inline deserializerDataStruct* operator()() { return NextDigit(); }

  uint32_t* NextUniqueID(bool loadAllData = false);

  inline const uint32_t* CurrentUniqueID() const { return &fUniqueID; }

  inline const uint32_t* GetCurrentData() const { return fData; }

  inline const uint32_t* GetHeader() const { return (uint32_t*)(fDataPtr); }

  inline const uint32_t* GetDataPointer() const { return fDigitsDataPtr; }

  inline const uint32_t GetNDigits() const { return fNDigits; }

  std::string PrintData() const
  {
    std::string outputString =
      "UID=" + std::to_string(fUniqueID) + " ElemID=" + std::to_string(fOutputDataStruct.fDetElemID) +
      " Bd=" + std::to_string(fOutputDataStruct.fBoardID) + " Ch=" + std::to_string(fOutputDataStruct.fChannel) +
      " Cat=" + std::to_string(fOutputDataStruct.fCathode);
    return outputString;
  };

 private:
  bool Advance();

  void Load();

  // Const values to exclude first 100 bytes of message and perform deserialization
  const uint32_t kHeaderLength = 25;

  uint32_t* fDataPtr;
  uint32_t* fDigitsDataPtr;
  uint32_t fNDigits;
  uint32_t fOffset;
  uint32_t fDigitCounter;
  uint32_t fUniqueID;
  uint32_t fData[2];

  // Internal data container
  deserializerDataStruct fOutputDataStruct;
};

} // namespace mid
} // namespace muon

} // namespace o2

#endif // DESERIALIZER_H
