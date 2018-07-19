// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/src/HitCounter.cxx
/// \brief  Hit counter algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   19 April 2018

#include "MIDFiltering/HitCounter.h"

namespace o2
{
namespace mid
{

void HitCounter::processData(const o2::mid::ColumnData& colData, const o2::mid::dataType dataType)
{
  /// Main function: runs on a data containing the strip patterns
  /// and accumulates the hits in counter objects
  /// @param colData input struct with deID, columnsID and patterns
  /// @param dataType defines if the input data is PHYS, FET or post-FET (noisy strips test)

  CounterContainer* containerPtr = nullptr;
  auto pairPtr = mContainersMap.find(colData.deId);
  // If the detector element is not there yet, create it and make containerPtr point at it
  if (pairPtr == mContainersMap.end()) {
    // Using emplace back power!!!
    mCounterContainers.emplace_back(CounterContainer{ colData.deId, {} });
    // Inserting the pair in the map
    mContainersMap.emplace(std::make_pair(colData.deId, mCounterContainers.size() - 1));
    // Assigning container to containerPtr as address of last added element
    containerPtr = &(mCounterContainers.back());
  } else {
    // Assigning container to containerPtr using the retrieved pair
    containerPtr = &(mCounterContainers[mContainersMap.find(colData.deId)->second]);
  }

  // Loop over the 5 received patterns and add them to the counters
  for (int iPattern = 0; iPattern < o2::mid::StripCounter::nCounters; ++iPattern) {
    // This += operator has been overloaded to perform a bitwise addition to 16 64 bit counters from a 16 bit pattern
    (*containerPtr)[dataType][colData.columnId].counters[iPattern] += colData.patterns[iPattern];
  }
}

void HitCounter::processDataVect(const std::vector<o2::mid::ColumnData>& colDataVect, const o2::mid::dataType dataType)
{
  /// Main function: runs on a vector of data containing ColumnData objects
  /// and accumulates the hits in counter objects
  /// @param colDataVect input vector of ColumnData
  /// @param dataType defines if the input data is PHYS, FET or post-FET (noisy strips test)

  // Looping over input vector calling processData on each element
  for (auto const& itData : colDataVect) {
    processData(itData, dataType);
  }
}

bool HitCounter::enoughToSend(const o2::mid::dataType dataType, const uint64_t minValue)
{
  /// Output condition: checks if there is enough statistics to send significative data
  /// @param dataType defines if the input data is PHYS, FET or post-FET (noisy strips test)
  /// @param minValue is the scalers threshold (applied per strip)

  bool enough = true;
  for (const auto& deIt : mCounterContainers) {
    enough &= deIt.enoughToSend(dataType, minValue);
  }
  return enough;
}

} // namespace mid
} // namespace o2
