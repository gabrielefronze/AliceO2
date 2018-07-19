// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/src/Filter.h
/// \brief  Digits filter algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   23 April 2018

#ifndef O2_MID_FILTER_H
#define O2_MID_FILTER_H

#include "DataFormatsMID/ColumnData.h"
#include <vector>
#include <unordered_map>

namespace o2
{
namespace mid
{
class Filter
{
 public:
  Filter() = default;
  ~Filter(){};
  Filter(Filter&) = delete;
  Filter(Filter&&) = delete;

  void loadMask(std::vector<o2::mid::MaskData> data);
  void maskData(o2::mid::ColumnData& data);
  void maskDataVect(std::vector<o2::mid::ColumnData>& dataVect);

  std::vector<o2::mid::MaskData> mNoisyDEIds;    ///< Container of masks
  std::unordered_map<uint8_t, size_t> mNoisyMap; ///< Map to acces faster the masks container
};
} // namespace mid
} // namespace o2

#endif // O2_MID_FILTER_H
