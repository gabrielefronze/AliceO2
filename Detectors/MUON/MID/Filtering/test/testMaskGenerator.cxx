// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/test/testMaskGenerator.h
/// \brief  Test mask generation algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   13 June 2018

#define BOOST_TEST_MODULE midFiltering
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "MIDFiltering/HitCounter.h"
#include "MIDFiltering/MaskGenerator.h"

using namespace o2::mid;

BOOST_AUTO_TEST_SUITE(testMaskGenerator)

BOOST_AUTO_TEST_CASE(LoadNoisy)
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

  auto mask = *(std::find_if(mg.mNoisyDEIds.begin(), mg.mNoisyDEIds.end(), [=](const MaskData& md) -> bool {
    return (md.deId == dummyColumnData.deId) && (md.columnId == dummyColumnData.columnId);
  }));

  BOOST_TEST(mask.deId == dummyColumnData.deId);

  uint iPat = 0;
  for (auto const& pat : patternsCol0) {
    std::cout << std::bitset<16>(mask.patterns[iPat]) << " mask represents NOT noisy channels in "
              << std::bitset<16>(pat) << std::endl;
    BOOST_TEST(mask.patterns[iPat] == (uint16_t)(~pat));
    iPat++;
  }
}

BOOST_AUTO_TEST_CASE(LoadDead)
{
  HitCounter hc;

  auto dummyColumnData = o2::mid::ColumnData();

  std::array<uint16_t,5> patternsCol0 = { 1, 29820, 56701, 44471, 1024 * 64 - 1 };

  dummyColumnData.columnId = 3;
  dummyColumnData.deId = 50;
  dummyColumnData.setPatterns(patternsCol0);

  hc.processData(dummyColumnData, dataType::DEAD);

  auto data = hc.getContainer(dummyColumnData.deId);

  MaskGenerator mg;

  mg.processData(hc.getContainer(dummyColumnData.deId), dataType::DEAD);

  auto mask = *(std::find_if(mg.mDeadDEIds.begin(), mg.mDeadDEIds.end(), [=](const MaskData& md) -> bool {
    return (md.deId == dummyColumnData.deId) && (md.columnId == dummyColumnData.columnId);
  }));

  BOOST_TEST(mask.deId == dummyColumnData.deId);

  uint iPat = 0;
  for (auto const& pat : patternsCol0) {
    std::cout << std::bitset<16>(mask.patterns[iPat]) << " mask represents dead channels in "
              << std::bitset<16>(pat) << std::endl;
    BOOST_TEST(mask.patterns[iPat] == (uint16_t)(~pat));
    iPat++;
  }
}

BOOST_AUTO_TEST_SUITE_END()