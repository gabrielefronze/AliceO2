// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/src/Decoder.cxx
/// \brief  Decoder-encoder to translate FEE data to ColumnData
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   24 April 2018

#include "Decoder.h"

namespace o2
{
namespace mid
{
std::vector<ColumnData> Decoder::decodeFEEData(const uint8_t* inputMsg)
{
  /// Main function: parses RCU input message into a vector of ColumnData
  /// @param inputMsg ptr to RCU message

  // Creating output vector
  std::vector<ColumnData> retVect;
  // Reset to {0,0} both offsets
  resetOffset();
  // Retrieve number of fired RPCs
  uint8_t nFiredRPCs = getNextNBits(inputMsg, 7);
  // Loop over fired RPCs
  for (int iRPC = 0; iRPC < nFiredRPCs; ++iRPC) {
    //Retrieve RPC ID
    uint8_t RPCId = getNextNBits(inputMsg, 7);
    // Retrieve number of fired columns
    uint8_t nFiredColumns = getNextNBits(inputMsg, 3);
    // Pre-allocate space in the output vector
    retVect.resize(retVect.size() + nFiredColumns);
    // Loop over fired columns
    for (int iColumn = 0; iColumn < nFiredColumns; ++iColumn) {
      // Emplace a new ColumnData element in output vector
      retVect.emplace_back(ColumnData());
      // Set deId and columnId of the last created ColumnData
      retVect.back().deId = RPCId;
      retVect.back().columnId = getNextNBits(inputMsg, 3);
      // Retrieve number of fired Local Boards (LB)
      uint8_t nFiredLBs = getNextNBits(inputMsg, 3);
      // patternNBPGlobal will contain the OR of all the patternNBP
      uint16_t patternNBPGlobal = 0;
      // copyErrorPattern will keep track of the strips presenting copy errors
      // since NBP patterns should be the same within the same column.
      uint16_t copyErrorPattern = 0;
      // Loop over fired LBs
      for (int iLB = 0; iLB < nFiredLBs; ++iLB) {
        // Retrieve LB ID
        uint8_t LBId = getNextNBits(inputMsg, 2);
        // Retrieve NBP pattern
        uint16_t patternNBP = getNextNBits(inputMsg, 16);
        // Bitwise OF with global NBP pattern
        patternNBPGlobal |= patternNBP;
        // Keep track of discrepancies between patterns
        copyErrorPattern |= (patternNBPGlobal ^ patternNBP);
        // Directly put BP pattern in the ColumnData object
        retVect.back().patterns.setBendPattern(getNextNBits(inputMsg, 16), LBId);
      }
      // Set NBP pattern using the global pattern
      retVect.back().patterns.setNonBendPattern(patternNBPGlobal);
    }
  }
  // Return by move the allocated vector
  return std::move(retVect);
}

} // namespace mid
} // namespace o2