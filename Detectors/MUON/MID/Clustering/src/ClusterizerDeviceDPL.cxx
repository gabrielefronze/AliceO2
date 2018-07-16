// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   ClusterizerDeviceDPL.h
/// \brief  Cluster reconstruction device for MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   6 July 2018

#include "include/MIDClustering/ClusterizerDeviceDPL.h"
#include "Framework/DPLBoostSerializer.h"

namespace o2
{
namespace mid
{
void ClusterizerDeviceDPL::init(o2::framework::InitContext& ic)
{
  if (!mClusterizer.init()) {
    LOG(ERROR) << "Initialization of MID clusterizer device failed";
  }
}

void ClusterizerDeviceDPL::run(o2::framework::ProcessingContext& pc)
{
  auto msg = pc.inputs().get(mInputBinding);

  std::vector<ColumnData> patterns;
  o2::framework::DPLBoostDeserialize<std::vector<ColumnData>>(msg, patterns);

  LOG(INFO) << "Processing clustering on " << patterns.size() << " ColumnDatas.";

  mClusterizer.process(patterns);

  LOG(INFO) << "Generated " << mClusterizer.getNClusters() << " Clusters.";

  if (mClusterizer.getNClusters() > 0) {
    o2::framework::DPLBoostSerialize(pc, mOutputBinding, mClusterizer.getClusters(), mClusterizer.getNClusters());
    LOG(INFO) << "Sent " << mClusterizer.getNClusters() << " Clusters.";
  }
}
} // namespace mid
} // namespace o2