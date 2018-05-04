// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#define BOOST_TEST_MODULE midSimulation
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/monomorphic/generators/xrange.hpp>
#include <boost/test/data/test_case.hpp>
#include <iostream>
#include <sstream>
#include <bitset>
#include "MathUtils/Cartesian3D.h"
#include "DataFormatsMID/ColumnData.h"
#include "MIDSimulation/Digitizer.h"
#include "MIDSimulation/ChamberResponseParams.h"
#include "MIDBase/Mapping.h"
#include "Clusterizer.h"

namespace bdata = boost::unit_test::data;

namespace o2
{
namespace mid
{

struct SIMUL {
  SIMUL()
  {
    clusterizer.init();
    params.setDefaultParams();
  }
  static o2::mid::Digitizer digitizer;
  static o2::mid::Clusterizer clusterizer;
  static o2::mid::Mapping mapping;
  static o2::mid::ChamberResponseParams params;
};

o2::mid::Digitizer SIMUL::digitizer;
o2::mid::Clusterizer SIMUL::clusterizer;
o2::mid::Mapping SIMUL::mapping;
o2::mid::ChamberResponseParams SIMUL::params;

BOOST_AUTO_TEST_SUITE(o2_mid_geometryTransformer)
BOOST_FIXTURE_TEST_SUITE(sim, SIMUL)

std::vector<Point3D<double>> generatePoints(int ntimes)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> distX(-127.5, 127.5);
  std::uniform_real_distribution<double> distY(-40., 40.);

  std::vector<Point3D<double>> points;

  for (int itime = 0; itime < ntimes; ++itime) {
    points.emplace_back(distX(mt), distY(mt), 0.);
  }

  return points;
}

// std::vector<int> getDeIds()
// {
//   o2::mid::ChamberResponseParams params;
//   params.setDefaultParams();
//   std::vector<int> deIdList;
//   for (int deId = 0; deId < 72; ++deId) {
//     if (params.getParB(0, deId) == params.getParB(1, deId)) {
//       deIdList.push_back(deId);
//     }
//   }
//   return deIdList;
// }

BOOST_DATA_TEST_CASE_F(SIMUL, MID_Digitizer, boost::unit_test::data::xrange(72) * generatePoints(100), deId, point)
// BOOST_DATA_TEST_CASE_F(SIMUL, MID_Digitizer, getDeIds() * generatePoints(100), deId, point)
{
  // In this test, we generate a cluster from one impact point and we reconstruct it.
  // If the impact point is in the RPC, the digitizer will return a list of strips,
  // that are close to each other by construction.
  // We will therfore have exactly one reconstructed cluster.
  // It is worth noting, however, that the clustering algorithm is desigend to
  // produce two clusters if the BP and NBP do not superpose.
  // The digitizer produces superposed BP and NBP strips only if the response parameters
  // are the same for both.
  // Otherwise we can have from 1 to 3 clusters produced.

  std::stringstream ss;
  int nGenClusters = 0, nRecoClusters = 0;
  if (mapping.stripByPosition(point.x(), point.y(), 0, deId, false).isValid()) {
    nGenClusters = 1;
    std::vector<ColumnData> digits = digitizer.hitToDigits(point, deId);
    clusterizer.process(digits);
    nRecoClusters = clusterizer.getNClusters();
    ss << "nRecoClusters: " << nRecoClusters << "  nGenClusters: " << nGenClusters << "\n";
    for (auto& col : digits) {
      ss << "col " << (int)col.columnId;
      for (int ip = 0; ip < 5; ++ip) {
        ss << " " << std::bitset<16>(col.patterns[ip]);
      }
    }
  }

  if (nGenClusters == 0 || (params.getParB(0, deId) == params.getParB(1, deId))) {
    BOOST_TEST((nRecoClusters == nGenClusters), ss.str());
  } else {
    BOOST_TEST((nRecoClusters >= nGenClusters && nRecoClusters <= 3), ss.str());
  }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace mid
} // namespace o2
