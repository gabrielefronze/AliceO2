// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   HitCounterSpec.cxx
/// \brief  Hit counter spec for MID reconstruction workflow
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#include "HitCounterSpec.h"
#include "MIDFiltering/HitCounterDeviceDPL.h"

namespace of = o2::framework;

namespace o2
{
namespace mid
{
framework::DataProcessorSpec getHitCounterSpec()
{
  std::string inputBinding = "mid_coldata";
  std::string outputBinding = "mid_scalers";

  auto outputChannel = of::Outputs{ of::OutputSpec{ "MID", "SCALERS" } };
  outputChannel.back().binding.value = outputBinding;

  return of::DataProcessorSpec{
    "HitCounter",
    of::Inputs{ of::InputSpec{ inputBinding, "MID", "COLDATA_to_HC" } },
    outputChannel,
    of::adaptFromTask<o2::mid::HitCounterDeviceDPL>(inputBinding, outputBinding)
  };
}
} // namespace mid
} // namespace o2
