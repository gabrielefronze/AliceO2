// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/test/testHitCounter.h
/// \brief  Test filter algorithm for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   23 April 2018

#define BOOST_TEST_MODULE midFiltering
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "HitCounter.h"

using namespace o2::mid;

BOOST_AUTO_TEST_SUITE(testHitCounter)

bool testPattern(CounterContainer& CC, dataType DT, uint columnId, uint iCounter, uint bit, uint16_t controlValue, uint16_t scalerValue = 0x1)
{
//  printf("Testing bit %d in pattern %d which should be %d but is %llu\n", bit, iCounter, scalerValue,
//         CC[DT][columnId].counters[iCounter][bit]);
  return (CC[DT][columnId].counters[iCounter][bit] == (((controlValue >> bit) & 0x1 ) * scalerValue));
}

BOOST_AUTO_TEST_CASE(LoadData)
{
  std::vector<ColumnData> columns;
  auto dummyColumnData = o2::mid::ColumnData();

  std::vector<uint16_t> patternsCol0 = { 0, 29820, 56701, 44471, 1024 * 64 - 1 };

  dummyColumnData.columnId = 0;
  dummyColumnData.deId = 0;
  dummyColumnData.setPatterns(patternsCol0);
  columns.emplace_back(dummyColumnData);

  HitCounter hc;

  hc.processData(columns.back(), dataType::PHYS);

  auto dataDe0 = hc.getContainer(dummyColumnData.deId);

  for (int iCounter = 0; iCounter < 5; ++iCounter) {
    for (int iBit = 0; iBit < 16; ++iBit) {
      BOOST_TEST(
        testPattern(dataDe0, dataType::PHYS, dummyColumnData.columnId, iCounter, iBit, patternsCol0[iCounter]));
    }
  }

  std::vector<uint16_t> patternsCol5 = { 21514, 7981, 47609, 45884, 37014 };

  dummyColumnData.columnId = 4;
  dummyColumnData.deId = 71;
  dummyColumnData.setPatterns(patternsCol5);
  columns.emplace_back(dummyColumnData);

  hc.processData(columns.back(), dataType::NOISY);

  auto dataDe71 = hc.getContainer(dummyColumnData.deId);

  for (int iCounter = 0; iCounter < 5; ++iCounter) {
    for (int iBit = 0; iBit < 16; ++iBit) {
      BOOST_TEST(
        testPattern(dataDe71, dataType::NOISY, dummyColumnData.columnId, iCounter, iBit, patternsCol5[iCounter]));
    }
  }
}

BOOST_AUTO_TEST_CASE(ScalerIncrement)
{
  std::vector<ColumnData> columns;
  auto dummyColumnData = o2::mid::ColumnData();

  std::vector<uint16_t> patternsCol0 = { 0, 29820, 56701, 44471, 1024 * 64 - 1 };

  dummyColumnData.columnId = 0;
  dummyColumnData.deId = 0;
  dummyColumnData.setPatterns(patternsCol0);
  columns.emplace_back(dummyColumnData);

  HitCounter hc;

  hc.processData(columns.back(), dataType::PHYS);
  hc.processData(columns.back(), dataType::PHYS);

  auto dataDe0 = hc.getContainer(dummyColumnData.deId);

  for (int iCounter = 0; iCounter < 5; ++iCounter) {
    for (int iBit = 0; iBit < 16; ++iBit) {
      BOOST_TEST(
        testPattern(dataDe0, dataType::PHYS, dummyColumnData.columnId, iCounter, iBit, patternsCol0[iCounter], 0x2));
    }
  }
}

BOOST_AUTO_TEST_CASE(DynamicDEAllocation) {
  std::vector<ColumnData> columns;
  auto dummyColumnData = o2::mid::ColumnData();

  dummyColumnData.columnId = 0;
  dummyColumnData.deId = 0;
  columns.emplace_back(dummyColumnData);

  HitCounter hc;

  BOOST_TEST(hc.mCounterContainers.size() == 0);
  BOOST_TEST(hc.mContainersMap.size() == 0);

  hc.processData(columns.back(), dataType::PHYS);

  BOOST_TEST(hc.mCounterContainers.size() == 1);
  BOOST_TEST(hc.mContainersMap.size() == 1);

  hc.processData(columns.back(), dataType::NOISY);

  BOOST_TEST(hc.mCounterContainers.size() == 1);
  BOOST_TEST(hc.mContainersMap.size() == 1);


  dummyColumnData.columnId = 4;
  dummyColumnData.deId = 71;
  columns.emplace_back(dummyColumnData);

  hc.processData(columns.back(), dataType::NOISY);

  BOOST_TEST(hc.mCounterContainers.size() == 2);
  BOOST_TEST(hc.mContainersMap.size() == 2);
}

BOOST_AUTO_TEST_SUITE_END()