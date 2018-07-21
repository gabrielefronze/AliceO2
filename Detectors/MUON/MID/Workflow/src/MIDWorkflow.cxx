// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDWorkflow.cxx
/// \brief  Definition of MID workflow
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   11 July 2018

#include <DataFormatsMID/ColumnData.h>
#include "Framework/WorkflowSpec.h"
#include "Framework/ConfigContext.h"
#include "Framework/runDataProcessing.h"
#include "Framework/DPLBoostSerializer.h"
#include "Utils/Utils.h"
#include "HitCounterSpec.h"
#include "MaskGeneratorSpec.h"
#include "FilterSpec.h"
#include "ClusterizerSpec.h"
#include "ColdataStreamerSpec.h"
#include "TrackerSpec.h"
#include "SinkSpec.h"

namespace of = o2::framework;

of::WorkflowSpec defineDataProcessing(const of::ConfigContext& configContext)
{
  of::WorkflowSpec workflow;

  workflow.emplace_back(o2::mid::getColdataStreamerSpec());
  workflow.emplace_back(o2::workflows::defineBroadcaster("ColdataBcast",
                                                         of::InputSpec{ "mid_coldata", "MID", "COLDATA" },
                                                         of::Outputs{ of::OutputSpec{ "MID", "COLDATA_to_F" },
                                                                      of::OutputSpec{ "MID", "COLDATA_to_HC" } }));
  workflow.emplace_back(o2::mid::getHitCounterSpec());
  workflow.emplace_back(o2::mid::getMaskGeneratorSpec());
  workflow.emplace_back(o2::mid::getFilterSpec());
  workflow.emplace_back(o2::mid::getClusterizerSpec());
  workflow.emplace_back(o2::mid::getTrackerSpec());
  workflow.emplace_back(o2::mid::getSinkSpec());

  return std::move(workflow);
}
