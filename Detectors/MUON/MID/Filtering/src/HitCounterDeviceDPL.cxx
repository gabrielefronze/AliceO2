// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   HitCounterDeviceDPL.cxx
/// \brief  Implementation of hit counter device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#include "MIDFiltering/HitCounterDeviceDPL.h"
#include "Framework/DPLBoostSerializer.h"
#include <vector>

namespace o2
{
namespace mid
{
void HitCounterDeviceDPL::run(o2::framework::ProcessingContext& pc)
{
  auto msg = pc.inputs().get(mInputBinding);
  std::vector<ColumnData> inputData;
  framework::DPLBoostDeserialize(msg, inputData);

  //TODO: retrieve data type from header or meta info
  dataType dt = dataType::PHYS;

  mHitCounter.processDataVect(inputData, dt);

  if (mHitCounter.enoughToSend(dt)) {
    framework::DPLBoostSerialize(pc, mOutputBinding, mHitCounter.getContainer(dt));
  }
}
} // namespace mid
} // namespace o2