// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDFiltering/HitCounterDeviceDPL.h
/// \brief  Implementation of hit counter device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#ifndef O2_MID_HITCOUNTERDEVICEDPL_H
#define O2_MID_HITCOUNTERDEVICEDPL_H

#include "MIDFiltering/HitCounter.h"
#include "Framework/Output.h"
#include "Framework/Task.h"
#include <vector>
#include <string>

namespace o2
{
namespace mid
{
class HitCounterDeviceDPL
{
 public:
  HitCounterDeviceDPL(std::string inputBinding,
                      std::string outputBinding) : mHitCounter()
  {
    mInputBinding = inputBinding;
    mOutputBinding = outputBinding;
  };

  ~HitCounterDeviceDPL(){};

  void init(o2::framework::InitContext& ic){};

  void run(o2::framework::ProcessingContext& pc);

 private:
  HitCounter mHitCounter;
  std::string mInputBinding;
  std::string mOutputBinding;
};
} // namespace mid

} // namespace o2

#endif //O2_MID_HITCOUNTERDEVICEDPL_H
