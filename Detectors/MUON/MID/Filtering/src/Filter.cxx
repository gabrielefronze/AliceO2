// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/src/Filter.cxx
/// \brief  Digits filter algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   23 April 2018

#include "MIDFiltering/Filter.h"

namespace o2
{
namespace mid
{
void Filter::loadMask(std::vector<o2::mid::MaskData> data)
{
  /// Load mask function: puts received masks in data members
  /// @param data vector of structs containing a struct of masks

  // Swap received data with internal container
  mNoisyDEIds.swap(data);
  // Reset internal map in order to rebuild it
  size_t position = 0;
  mNoisyMap.clear();
  // Rebuild iteratively the map
  for (const auto& itNoisy : mNoisyDEIds) {
    mNoisyMap.emplace(std::make_pair(itNoisy.deId, position));
    position++;
  }
}

void Filter::maskData(o2::mid::ColumnData& data)
{
  /// Masking function: masks and forwards one ColumnData object
  /// @param data column data to be masked with data members information

  // Get the iterator to the required map
  auto maskFinder = mNoisyMap.find(data.deId);
  // If detector element is found proceed masking, else forward data
  if (maskFinder != mNoisyMap.end()) {
    // Get the required mask
    auto& mask = mNoisyDEIds[(*maskFinder).second].patterns;
    // Loop over input bitpatterns and perform bitwise AND with mask data
    for (int iMask = 0; iMask < mask.size(); ++iMask) {
      data.patterns[iMask] &= mask[iMask];
    }
  }
}

void Filter::maskDataVect(std::vector<o2::mid::ColumnData>& dataVect)
{
  /// Masking function: masks and forwards a vector of ColumnData objects
  /// @param dataVect a vector column data to be masked with data members information

  // Looping over input vector to mask it in-place.
  // Note: this is not a const reference, since the patterns are being masked hence modified!
  for (auto& itData : dataVect) {
    maskData(itData);
  }
}

} // namespace mid
} // namespace o2