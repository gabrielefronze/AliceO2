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
///  @brief  Little deserializer app to pack MID messages
///

#ifndef O2_MID_SERIALIZER_H
#define O2_MID_SERIALIZER_H

#include <iostream>
#include <vector>
#include "DataStructs.h"

namespace o2
{
namespace mid
{
class Serializer
{
 public:
  Serializer();

  ~Serializer(){};

  inline void AddDigit(uint32_t detElemID, uint32_t boardID, uint32_t channel, uint32_t cathode)
  {
    fData.emplace_back(deserializerDataStruct(detElemID, boardID, channel, cathode));
    fData.emplace_back(deserializerDataStruct(0, 0, 0, 0));
  };

  inline void AddDigit(deserializerDataStruct dataStruct)
  {
    fData.emplace_back(dataStruct);
    fData.emplace_back(deserializerDataStruct(0, 0, 0, 0));
  };

  uint32_t GetUID(deserializerDataStruct dataStruct);

  uint32_t GetUID(size_t index);

  uint32_t* GetMessage();
  inline size_t GetMessageSize() {
    return sizeof(uint32_t)*(fHeader.size()+1+fOutputData.size()); //The +1 is the value representing the number of digits
  }

  inline void DumpHeader()
  {
    for (const auto& it : fHeader)
      std::cout << it << std::endl;
  };

 private:
  // Const values to exclude first 100 bytes of message and perform deserialization
  const uint32_t kHeaderLength = 25;
  std::vector<uint32_t> fHeader;
  // This vector will contain the full output message
  std::vector<uint32_t> fOutputData;

  // Internal data container
  std::vector<deserializerDataStruct> fData;
};
} // namespace mid
} // namespace muon

#endif // O2_MID_SERIALIZER_H
