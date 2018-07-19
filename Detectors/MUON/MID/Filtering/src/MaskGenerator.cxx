// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/src/MaskGenerator.cxx
/// \brief  Mask generator algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   20 April 2018

#include <iostream>
#include "MIDFiltering/MaskGenerator.h"

namespace o2
{
namespace mid
{

void MaskGenerator::processData(std::vector<CounterContainer> data, dataType dataType)
{
  for(auto& itData : data){
    this->processData(itData,dataType);
  }
}

void MaskGenerator::processData(CounterContainer data, dataType dataType)
{
  /// Main function: calls the right function to process input data
  /// @param data input struct containing per-strip scalers
  /// @param dataType defines if the input data is PHYS, FET or post-FET (noisy strips test)

  switch (dataType) {
    case dataType::DEAD:
      detectDead(data.deId, std::move(data[dataType]));
    case dataType::NOISY:
      detectNoisy(data.deId, std::move(data[dataType]));
    default:
      return;
  }
}

void MaskGenerator::detectDead(uint8_t deId, CounterStruct data)
{
  /// Dead strips detection function: finds dead strips looking at 0 counters
  /// @param deId is the detector element ID which identifies the current RPC
  /// @param data input struct containing per-strip scalers

  uint8_t columnId = 0;
  // Loop over DE columns
  for (auto const& itColumn : data.columns) { // 0->7
    MaskData maskData{};
    maskData.deId = deId;
    // Loading column ID in maskData
    maskData.columnId = columnId;
    // Counter for mask shifting
    size_t iLB = 0;
    // Loop over LBs in current column
    for (auto const &itLocalBoard : itColumn.counters) { // 0->5
      // Bit index
      size_t iBit = 0;
      // Loop over counters
      for (auto const &itScalers : itLocalBoard.counter) { // 0->16
        // Setting to 1 the bit corresponding to dead strips
        maskData.patterns[iLB] |= (itScalers == 0) << iBit;
        // Incrementing bit shift
        iBit++;
      }
      // Incrementing LB index
      iLB++;
    }
    columnId++;

    bool hasDead = false;
    // Checking if column has some dead strips
    for (auto const &itPatterns : maskData.patterns) {
      hasDead |= (itPatterns > 0);
    }

    // If some strip(s) is(are) noisy save the mask in the data member
    if (hasDead) {
      // Try to find the deId in the data member
      auto foundPtr = std::find_if(mDeadDEIds.begin(), mDeadDEIds.end(),
                                   [deId = maskData.deId, colId = columnId](MaskData data) { return (data.deId == deId)&&(data.columnId == colId); });
      // If not found simply emplace back
      if (foundPtr == mDeadDEIds.end())
        mDeadDEIds.emplace_back(std::move(maskData));
        // Else replace by move the current version
      else
        *foundPtr = std::move(maskData);
    }
  }
}

void MaskGenerator::detectNoisy(uint8_t deId, CounterStruct data)
{
  uint8_t columnId = 0;
  // Loop over DE columns
  for (auto const& itColumn : data.columns) { // 0->7
    MaskData maskData{};
    maskData.deId = deId;
    // Loading column ID in maskData
    maskData.columnId = columnId;
    // Counter for mask shifting
    size_t iLB = 0;
    // Loop over LBs in current column
    for (auto const& itLocalBoard : itColumn.counters) { // 0->5
      // Bit index
      size_t iBit = 0;
      // Loop over counters
      maskData.patterns[iLB] = 0;
      for (auto const& itScalers : itLocalBoard.counter) { // 0->16
        // Setting to 1 the bit corresponding to noisy strips
        maskData.patterns[iLB] |= (itScalers != 0) << iBit;
        // Incrementing bit shift
        iBit++;
      }
      // Performing bitwise NOT (to perform a bitwise AND as masking operation)
      maskData.patterns[iLB] = ~(maskData.patterns[iLB]);

      // Incrementing LB index
      iLB++;
    }

    bool hasNoisy = false;
    // Checking if column has some dead strips
    for (auto const& itPatterns : maskData.patterns) {
      hasNoisy |= (itPatterns != ((1 << 16)-1));
    }

    // If some strip(s) is(are) noisy save the mask in the data member
    if (hasNoisy) {
      // Try to find the deId in the data member
      auto foundPtr = std::find_if(mNoisyDEIds.begin(), mNoisyDEIds.end(),
                                   [deId = maskData.deId, colId = columnId](MaskData data) { return (data.deId == deId)&&(data.columnId == colId); });
      // If not found simply emplace back
      if (foundPtr == mNoisyDEIds.end())
        mNoisyDEIds.emplace_back(std::move(maskData));
      // Else replace by move the current version
      else
        *foundPtr = std::move(maskData);
    }

    columnId++;
  }
}

} // namespace mid
} // namespace o2
