// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   SinkSpec.cxx
/// \brief  Data processor spec for data sink device
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   11 July 2018

#include "Framework/DataProcessorSpec.h"
#include "Framework/InputSpec.h"
#include "Framework/AlgorithmSpec.h"
#include "DataFormatsMID/Track.h"
#include "Framework/DPLBoostSerializer.h"
#include "SinkSpec.h"

namespace of = o2::framework;

namespace o2
{
namespace mid
{
framework::DataProcessorSpec getSinkSpec()
{
  return of::DataProcessorSpec{
    "Sink",
    of::Inputs{ of::InputSpec{ "in_data", "MID", "TRACKS" } },
    of::Outputs{},
    of::AlgorithmSpec{
      [](of::ProcessingContext& pc) {
        std::vector<Track> dataVect;
        o2::framework::DPLBoostDeserialize(pc.inputs().get("in_data"), dataVect);
        LOG(INFO) << "Getting rid of Tracks.";
      } }
  };
};
} // namespace mid
} // namespace o2