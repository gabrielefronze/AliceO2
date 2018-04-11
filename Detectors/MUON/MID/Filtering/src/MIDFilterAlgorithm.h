//
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @author  Gabriele Gaetano Fronzé
///  @file   MIDFilterAlgorithm
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   30 Jan 2018
///  @brief  Algorithm to compute rates based on occupancy information for MID
///

#ifndef MIDFILTERALGORITHM_H
#define MIDFILTERALGORITHM_H

#include "MUONBase/DataStructs.h"
#include <vector>
#include <dtrace.h>

namespace o2
{
namespace muon
{
namespace mid
{
class MIDFilterAlgorithm
{
  public:
    MIDFilterAlgorithm();

    bool Init();
    bool ExecFilter(std::vector<uint32_t> data);
    bool ExecMaskLoading(ushort_t*maskHeader, uint32_t *maskData);

 private:
  stripMask fMask;
};
} // namespace mid
} // namespace muon
} // namespace o2

#endif // MIDFILTERALGORITHM_H
