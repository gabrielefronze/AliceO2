// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDFiltering/FilterDeviceDPL.cxx
/// \brief  Implementation of filter device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#include "MIDFiltering/FilterDeviceDPL.h"
#include "Framework/DPLBoostSerializer.h"

namespace o2
{
namespace mid
{
void FilterDeviceDPL::run(o2::framework::ProcessingContext& pc)
{
  auto msgMask = pc.inputs().get(mInputBindingMask);
  std::vector<MaskData> inputMask;
  framework::DPLBoostDeserialize(msgMask, inputMask);

  mFilter.loadMask(inputMask);

  auto msgData = pc.inputs().get(mInputBindingMask);
  std::vector<ColumnData> inputData;
  framework::DPLBoostDeserialize(msgData, inputData);

  mFilter.maskDataVect(inputData);

  auto newEnd = std::remove_if(std::begin(inputData), std::end(inputData), [](ColumnData& cd)->bool{
    auto nNotZero = 0;
    for(auto const& itPat : cd.patterns){
      nNotZero += (itPat!=0);
    }
    return (nNotZero>0);
  });

  framework::DPLBoostSerialize(pc, mOutputBinding, inputData, std::distance(std::begin(inputData), newEnd));
}
} // namespace mid
} // namespace o2