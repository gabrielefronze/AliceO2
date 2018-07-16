// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   ClusterizerSpec.cxx
/// \brief  Data processor spec for MID clustering device
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   9 July 2018

#include <MIDClustering/ClusterizerDeviceDPL.h>
#include "Framework/DataProcessorSpec.h"
#include "Framework/Task.h"
#include "Framework/Output.h"
#include "ClusterizerSpec.h"

namespace of = o2::framework;

namespace o2
{
namespace mid
{
framework::DataProcessorSpec getClusterizerSpec()
{
  std::string inputBinding = "mid_coldata";
  std::string outputBinding = "mid_clusters";

  auto outputChannel = of::Outputs{ of::OutputSpec{ "MID", "CLUSTERS" } };
  outputChannel.back().binding.value = outputBinding;


  return of::DataProcessorSpec{
    "Clusterizer",
    of::Inputs{ of::InputSpec{ inputBinding, "MID", "COLDATA_F" } },
    outputChannel,
    of::adaptFromTask<o2::mid::ClusterizerDeviceDPL>(inputBinding, outputBinding)
  };
}
} // namespace mid
} // namespace o2