// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/src/HitCounter.h
/// \brief  Hit counter algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   19 April 2018

#ifndef O2_MID_HITCOUNTER_H
#define O2_MID_HITCOUNTER_H

#include "DataFormatsMID/StripCounter.h"
#include "DataFormatsMID/ColumnData.h"
#include <vector>
#include <unordered_map>

namespace o2
{
namespace mid
{
class HitCounter
{
 public:
  HitCounter() = default;
  ~HitCounter(){};
//  HitCounter(HitCounter&) = delete;
//  HitCounter(HitCounter&&) = delete;

  void processData(const o2::mid::ColumnData& colData, const o2::mid::dataType dataType);
  void processDataVect(const std::vector<o2::mid::ColumnData>& colData, const o2::mid::dataType dataType);
  bool enoughToSend(const dataType dataType, const uint64_t minValue = 1);

  const o2::mid::CounterContainer& getContainer(size_t index){ return mCounterContainers[mContainersMap[index]]; };

  std::vector<o2::mid::CounterContainer> mCounterContainers; ///< container of hit counters, one per detection element
  std::unordered_map<uint8_t, size_t> mContainersMap;        ///< map referring to mCounterContainers
};
} // namespace mid
} // namespace o2

#endif // O2_MID_HITCOUNTER_H
