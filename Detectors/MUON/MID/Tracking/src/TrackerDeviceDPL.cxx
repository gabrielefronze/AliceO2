// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Tracking/src/TrackerDevice.cxx
/// \brief  Implementation of tracker device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   6 July 2018

#include "MIDTracking/TrackerDeviceDPL.h"
#include "Framework/DPLBoostSerializer.h"

namespace o2
{
namespace mid
{
void TrackerDeviceDPL::run(o2::framework::ProcessingContext& pc)
{
  auto msg = pc.inputs().get(mInputBinding);

  std::vector<Cluster2D> clusters;
  o2::framework::DPLBoostDeserialize<std::vector<Cluster2D>>(msg, clusters);

  LOG(INFO) << "Processing tracking on " << clusters.size() << " Clusters.";

  mTracker.process(clusters);

  if (mTracker.getNTracks() > 0) {
    o2::framework::DPLBoostSerialize(pc, mOutputBinding, mTracker.getTracks(), mTracker.getNTracks());
  }
}
} // namespace mid
} // namespace o2