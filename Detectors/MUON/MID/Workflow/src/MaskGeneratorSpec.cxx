// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MaskgeneratorSpec.cxx
/// \brief  Mask generator spec for MID reconstruction workflow
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#include "MaskGeneratorSpec.h"
#include "MIDFiltering/MaskGeneratorDeviceDPL.h"

namespace of = o2::framework;

namespace o2
{
namespace mid
{
framework::DataProcessorSpec getMaskGeneratorSpec()
{
  std::string inputBinding = "mid_scalers";
  std::string outputBinding = "mid_mask";

  auto outputChannel = of::Outputs{ of::OutputSpec{ "MID", "MASK" } };
  outputChannel.back().binding.value = outputBinding;

  return of::DataProcessorSpec{
    "MaskGenerator",
    of::Inputs{ of::InputSpec{ inputBinding, "MID", "SCALERS" } },
    outputChannel,
    of::adaptFromTask<o2::mid::MaskGeneratorDeviceDPL>(inputBinding, outputBinding)
  };
}
} // namespace mid
} // namespace o2
