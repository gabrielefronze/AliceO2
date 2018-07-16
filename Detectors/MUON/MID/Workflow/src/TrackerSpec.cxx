// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   Tracker.cxx
/// \brief  Data processor spec for MID tracker device
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   9 July 2018

#include <MIDTracking/TrackerDeviceDPL.h>
#include "Framework/DataProcessorSpec.h"
#include "Framework/Task.h"
#include "TrackerSpec.h"

namespace of = o2::framework;

namespace o2
{
namespace mid
{
framework::DataProcessorSpec getTrackerSpec()
{
  std::string inputBinding = "mid_clusters";
  std::string outputBinding = "mid_tracks";

  auto outputChannel = of::Outputs{ of::OutputSpec{ "MID", "TRACKS" } };
  outputChannel.back().binding.value = outputBinding;

  return of::DataProcessorSpec{
    "Tracker",
    of::Inputs{ of::InputSpec{ inputBinding, "MID", "CLUSTERS" } },
    outputChannel,
    of::adaptFromTask<o2::mid::TrackerDeviceDPL>(inputBinding, outputBinding)
  };
}
} // namespace mid
} // namespace o2