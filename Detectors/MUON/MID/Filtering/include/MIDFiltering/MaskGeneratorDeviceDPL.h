// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDFiltering/MaskGenaratorDeviceDPL.h
/// \brief  Implementation of mask generator device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#ifndef O2_MID_MASKGENERATORDEVICEDPL_H
#define O2_MID_MASKGENERATORDEVICEDPL_H

#include "MIDFiltering/MaskGenerator.h"
#include "Framework/Output.h"
#include "Framework/Task.h"
#include <vector>
#include <string>

namespace o2
{
namespace mid
{
class MaskGeneratorDeviceDPL
{
 public:
  MaskGeneratorDeviceDPL(std::string inputBinding,
                         std::string outputBinding) : mMaskGenerator()
  {
    mInputBinding = inputBinding;
    mOutputBindingFilter = outputBinding;
  };

  ~MaskGeneratorDeviceDPL(){};

  void init(o2::framework::InitContext& ic)
  {
    mMaskGenerator.resetAll();
  };

  void run(o2::framework::ProcessingContext& pc);

 private:
  MaskGenerator mMaskGenerator;
  std::string mInputBinding;
  std::string mOutputBindingFilter;
};
} // namespace mid
} // namespace o2

#endif //O2_MID_MASKGENERATORDEVICEDPL_H
