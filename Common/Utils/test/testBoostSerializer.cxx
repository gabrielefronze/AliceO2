//
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @author  Gabriele Gaetano Fronzé

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <iostream>
#include <vector>
#include "CommonUtils/BoostSerializer.h"
#include "DataFormatsMID/Cluster2D.h"

using namespace o2::utils;

BOOST_AUTO_TEST_SUITE(testDPLSerializer)

BOOST_AUTO_TEST_CASE(testTrivialTypeVect)
{
  using contType = std::vector<int>;

  contType inputV{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

  auto msgStr = SerializeContainer(inputV).str();
  auto inputV2 = DeserializeContainer<contType>(msgStr);

  BOOST_TEST(inputV.size() == inputV2.size());

  size_t i = 0;
  for (auto const& test : inputV) {
    BOOST_TEST(test == inputV2[i]);
    i++;
  }
}

BOOST_AUTO_TEST_CASE(testTrivialTypeArray)
{
  using contType = std::array<int, 20>;

  contType inputV{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

  auto msgStr = SerializeContainer(inputV).str();
  auto inputV2 = DeserializeContainer<contType>(msgStr);

  BOOST_TEST(inputV.size() == inputV2.size());

  size_t i = 0;
  for (auto const& test : inputV) {
    BOOST_TEST(test == inputV2[i]);
    i++;
  }
}

BOOST_AUTO_TEST_CASE(testTrivialTypeList)
{
  using contType = std::list<int>;

  contType inputV{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

  auto msgStr = SerializeContainer(inputV).str();
  auto inputV2 = DeserializeContainer<contType>(msgStr);

  BOOST_TEST(inputV.size() == inputV2.size());

  size_t i = 0;
  for (auto const& test : inputV) {
    auto value = std::next(std::begin(inputV2), i).operator*();
    BOOST_TEST(test == value);
    i++;
  }
}

BOOST_AUTO_TEST_CASE(testBoostSerialisedType)
{
  using contType = std::vector<o2::mid::Cluster2D>;

  contType inputV;

  for (size_t i = 0; i < 17; i++) {
    float iFloat = (float)i;
    inputV.emplace_back(o2::mid::Cluster2D{ (uint8_t)i, 0.3f * iFloat, 0.5f * iFloat, 0.7f / iFloat, 0.9f / iFloat });
  }

  auto msgStr = SerializeContainer(inputV).str();
  auto inputV2 = DeserializeContainer<contType>(msgStr);

  BOOST_TEST(inputV.size() == inputV2.size());

  size_t i = 0;
  for (auto const& test : inputV) {
    BOOST_TEST(test.deId == inputV2[i].deId);
    BOOST_TEST(test.xCoor == inputV2[i].xCoor);
    BOOST_TEST(test.yCoor == inputV2[i].yCoor);
    BOOST_TEST(test.sigmaX2 == inputV2[i].sigmaX2);
    BOOST_TEST(test.sigmaY2 == inputV2[i].sigmaY2);
    i++;
  }
}

BOOST_AUTO_TEST_SUITE_END()