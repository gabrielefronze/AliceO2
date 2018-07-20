// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDFiltering/FilterDeviceDPL.h
/// \brief  Implementation of filtering device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#ifndef O2_MID_FILTERDEVICEDPL_H
#define O2_MID_FILTERDEVICEDPL_H

#include "MIDFiltering/Filter.h"
#include "Framework/Output.h"
#include "Framework/Task.h"
#include <vector>
#include <string>

namespace o2
{
namespace mid
{
class FilterDeviceDPL
{
 public:
  FilterDeviceDPL(std::string inputBindingMask,
                  std::string inputBindingColdata,
                  std::string outputBinding) : mFilter()
  {
    mInputBindingMask = inputBindingMask;
    mInputBindingColdata = inputBindingColdata;
    mOutputBinding = outputBinding;
  };

  ~FilterDeviceDPL(){};

  void init(o2::framework::InitContext& ic){};

  void run(o2::framework::ProcessingContext& pc);

 private:
  Filter mFilter;
  std::string mInputBindingMask;
  std::string mInputBindingColdata;
  std::string mOutputBinding;
};
} // namespace mid
} // namespace o2

#endif //O2_MID_FILTERDEVICEDPL_H
