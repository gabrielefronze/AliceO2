// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDFiltering/MaskGenaratorDeviceDPL.cxx
/// \brief  Implementation of mask generator device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#include "MIDFiltering/MaskGeneratorDeviceDPL.h"
#include "Framework/DPLBoostSerializer.h"

namespace o2
{
namespace mid
{
void MaskGeneratorDeviceDPL::run(o2::framework::ProcessingContext& pc)
{
  auto msg = pc.inputs().get(mInputBinding);
  std::vector<CounterContainer> inputData;
  framework::DPLBoostDeserialize(msg, inputData);

  //TODO: retrieve data type from header or meta info
  dataType dt = dataType::DEAD;

  mMaskGenerator.processData(inputData, dt);

  //Sending data to Filter in order to apply the masks
  framework::DPLBoostSerialize(pc, mOutputBindingFilter, mMaskGenerator.mNoisyDEIds);

  //TODO: dead and noisy strips and masks have to be sent to CDB
  //  framework::DPLBoostSerialize(pc,mOutputBindingCDB,mMaskGenerator.mNoisyDEIds);
  //  framework::DPLBoostSerialize(pc,mOutputBindingCDB,mMaskGenerator.mDeadDEIds);

  //TODO: masks have to be sent to FEE to be applied directly
  //  framework::DPLBoostSerialize(pc,mOutputBindingFEE,mMaskGenerator.mNoisyDEIds);
}
} // namespace mid
} // namespace o2