// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/src/Decoder.h
/// \brief  Decoder-encoder to translate FEE data to ColumnData
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   24 April 2018

#ifndef O2_MID_DECODER_H
#define O2_MID_DECODER_H

#include <cstdint>
#include <vector>
#include "DataFormatsMID/StripPattern.h"

namespace o2
{
namespace mid
{
class Decoder
{
 public:
  std::vector<ColumnData> decodeFEEData(const uint8_t* inputmsg);

 private:
  int mBitOffset{ 0 };
  int mByteOffset{ 0 };

  inline void resetOffset()
  {
    /// Reset function: resets internal offset counters
    mBitOffset = 0;
    mByteOffset = 0;
  }

  uint64_t getNextNBits(const uint8_t* inputMsg, const int nBits)
  {
    /// Masking function: returns the last nBits of shifted inputMsg. Shift is stored in data members.
    /// @param inputMsg ptr to raw CRU message
    /// @param nBits number of bits to retrieve
    // Get trailing nBits by masking with 2^nBits-1 a properly shifted inputMsg
    // mByteOffset corresponds to uint8_t array positions
    // mBitOffset describes below-byte shifts
    // static cast to bit int type is needed to allow to read 7-bit shifted 32-bit integers (39-bit significance)
    uint64_t retValue = (static_cast<uint64_t>(inputMsg[mByteOffset]) >> mBitOffset) & ((1 << nBits) - 1);
    // mByteOffset is updated (incremented)
    mByteOffset += mBitOffset / 8;
    // mBitOffset is the remider of previous operation
    mBitOffset = mBitOffset % 8;
    // return by moving the result
    return std::move(retValue);
  }
};
} // namespace mid
} // namespace o2

#endif // O2_MID_DECODER_H
