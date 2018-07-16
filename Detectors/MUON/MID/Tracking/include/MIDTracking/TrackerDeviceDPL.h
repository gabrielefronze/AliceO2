// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Tracking/src/TrackerDevice.cxx
/// \brief  Implementation of tracker device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   6 July 2018

#ifndef O2_MID_TRACKERDEVICEDPL_H
#define O2_MID_TRACKERDEVICEDPL_H

#include "Tracker.h"
#include "DataFormatsMID/Cluster2D.h"
#include "DataFormatsMID/Track.h"
#include "Framework/Output.h"
#include "Framework/Task.h"
#include <vector>

namespace o2
{
namespace mid
{
class TrackerDeviceDPL
{
 public:
  TrackerDeviceDPL(std::string inputBinding,
                   std::string outputBinding) : mTracker()
  {
    mInputBinding = inputBinding;
    mOutputBinding = outputBinding;
  };
  ~TrackerDeviceDPL(){};

  void init(o2::framework::InitContext& ic){};
  void run(o2::framework::ProcessingContext& pc);

 private:
  Tracker mTracker;
  std::string mInputBinding;
  std::string mOutputBinding;
};
} // namespace mid
} // namespace o2

#endif /* O2_MID_TRACKERDEVICEDPL_H */