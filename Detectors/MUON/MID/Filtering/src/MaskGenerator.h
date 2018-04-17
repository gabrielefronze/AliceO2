// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/src/MaskGenerator.h
/// \brief  Mask generator algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   20 April 2018

#ifndef O2_MID_MASKGENERATOR_H
#define O2_MID_MASKGENERATOR_H

#include "DataFormatsMID/StripCounter.h"
#include "DataFormatsMID/StripPattern.h"
#include <vector>
#include <unordered_map>

namespace o2
{
namespace mid
{
class MaskGenerator
{
 public:
  MaskGenerator() = default;
  ~MaskGenerator(){};
//  MaskGenerator(MaskGenerator&) = delete;
//  MaskGenerator(MaskGenerator&&) = delete;

  void processData(o2::mid::CounterContainer data, o2::mid::dataType dataType);
  void detectNoisy(uint8_t deId, o2::mid::CounterStruct data);
  void detectDead(uint8_t deId, o2::mid::CounterStruct data);

  std::vector<o2::mid::CounterContainer> mCounterContainers; ///< container of hit counters, one per detection element
  std::unordered_map<uint8_t, size_t> mContainersMap;        ///< map referring to mCounterContainers

  std::vector<o2::mid::MaskData> mNoisyDEIds; ///< This patterns' set is a mask: bit is 1 when the data must be kept
  std::vector<o2::mid::ColumnData> mDeadDEIds; ///< This patterns' set is a pattern:  bit 1 identifies dead channels
};
} // namespace mid
} // namespace o2

#endif // O2_MID_MASKGENERATOR_H
