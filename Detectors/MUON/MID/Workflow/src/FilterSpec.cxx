// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   FilterSpec.cxx
/// \brief  Filter spec for MID reconstruction workflow
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   20/07/2018

#include "FilterSpec.h"
#include "MIDFiltering/FilterDeviceDPL.h"

namespace of = o2::framework;

namespace o2
{
namespace mid
{
framework::DataProcessorSpec getFilterSpec()
{
  std::string inputBindingMask = "mid_mask";
  std::string inputBindingColdata = "mid_coldata";
  std::string outputBinding = "mid_coldata_f";

  auto outputChannel = of::Outputs{ of::OutputSpec{ "MID", "COLDATA_F" } };
  outputChannel.back().binding.value = outputBinding;

  return of::DataProcessorSpec{
    "Filter",
    of::Inputs{ of::InputSpec{ inputBindingMask, "MID", "MASK" },
                of::InputSpec{ inputBindingColdata, "MID", "COLDATA_to_F" } },
    outputChannel,
    of::adaptFromTask<o2::mid::FilterDeviceDPL>(inputBindingMask, inputBindingColdata, outputBinding)
  };
}
} // namespace mid
} // namespace o2
