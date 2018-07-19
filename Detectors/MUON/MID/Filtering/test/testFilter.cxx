// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/test/testFilter.h
/// \brief  Test filtering algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   26 June 2018

#define BOOST_TEST_MODULE midFiltering
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "MIDFiltering/HitCounter.h"
#include "MIDFiltering/MaskGenerator.h"
#include "MIDFiltering/Filter.h"

using namespace o2::mid;

BOOST_AUTO_TEST_SUITE(testFilter)

BOOST_AUTO_TEST_CASE(LoadMask)
{
  HitCounter hc;

  auto dummyColumnData = o2::mid::ColumnData();
  std::array<uint16_t,5> patternsCol0 = { 1, 29820, 56701, 44471, 1024 * 64 - 1 };
  dummyColumnData.columnId = 4;
  dummyColumnData.deId = 62;
  dummyColumnData.setPatterns(patternsCol0);

  hc.processData(dummyColumnData, dataType::NOISY);

  auto data = hc.getContainer(dummyColumnData.deId);

  MaskGenerator mg;

  mg.processData(hc.getContainer(dummyColumnData.deId), dataType::NOISY);

  Filter phil;

  phil.loadMask(mg.mNoisyDEIds);

  BOOST_TEST(phil.mNoisyDEIds.size() == 1);
  auto findIt = std::find_if(phil.mNoisyDEIds.begin(), phil.mNoisyDEIds.end(), [=](const ColumnData& CD) -> bool {
    return (CD.deId == dummyColumnData.deId) && (CD.columnId == dummyColumnData.columnId);
  });
  BOOST_TEST(!(findIt == phil.mNoisyDEIds.end()));

  auto notfoundIt = std::find_if(phil.mNoisyDEIds.begin(), phil.mNoisyDEIds.end(), [=](const ColumnData& CD) -> bool {
    return (CD.deId == dummyColumnData.deId+1) && (CD.columnId == dummyColumnData.columnId+1);
  });
  BOOST_TEST((bool)(notfoundIt == phil.mNoisyDEIds.end()));
}

BOOST_AUTO_TEST_CASE(ApplyMask)
{
  HitCounter hc;

  auto dummyColumnData = o2::mid::ColumnData();
  std::array<uint16_t,5> patternsCol0 = { 1, 29820, 56701, 44471, 1024 * 64 - 1 };
  dummyColumnData.columnId = 4;
  dummyColumnData.deId = 62;
  dummyColumnData.setPatterns(patternsCol0);

  hc.processData(dummyColumnData, dataType::NOISY);

  auto data = hc.getContainer(dummyColumnData.deId);

  MaskGenerator mg;

  mg.processData(hc.getContainer(dummyColumnData.deId), dataType::NOISY);

  Filter phil;

  phil.loadMask(mg.mNoisyDEIds);

  auto rawData = o2::mid::ColumnData();
  std::array<uint16_t,5> rawDataPatterns = { 0xFFFF, 0xBACD, 0xFFFF, 0x1234, 0xFFFF };
  rawData.columnId = 4;
  rawData.deId = 62;
  rawData.setPatterns(rawDataPatterns);

  auto maskedData = rawData;

  phil.maskData(maskedData);

  size_t iMask = 0;
  for (auto const& mskPatternIt : maskedData.patterns) {
    std::cout << std::bitset<16>(rawDataPatterns[iMask]) << " is masked through "
              << std::bitset<16>(patternsCol0[iMask]) << " giving " << std::bitset<16>(mskPatternIt) << std::endl;
    BOOST_TEST(mskPatternIt == (uint16_t)(rawDataPatterns[iMask] & ~(patternsCol0[iMask])));
    iMask++;
  }
}

BOOST_AUTO_TEST_CASE(ApplyEmptyMask)
{
  MaskGenerator mg;

  Filter phil;

  phil.loadMask(mg.mNoisyDEIds);

  auto rawData = o2::mid::ColumnData();
  std::array<uint16_t,5> rawDataPatterns = { 0xFFFF, 0xABCD, 0xFFFF, 0x1234, 0xFFFF };
  rawData.columnId = 4;
  rawData.deId = 62;
  rawData.setPatterns(rawDataPatterns);

  auto maskedData = rawData;

  phil.maskData(maskedData);

  size_t iPat = 0;
  for (auto const& mskPatternIt : maskedData.patterns) {
    std::cout << std::bitset<16>(rawDataPatterns[iPat]) << " is masked through empty mask giving "
              << std::bitset<16>(mskPatternIt) << std::endl;
    BOOST_TEST(mskPatternIt == rawData.patterns[iPat]);
    iPat++;
  }
}

BOOST_AUTO_TEST_SUITE_END()