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
///  @file   MIDRatesComputer
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Algorithm to compute rates based on occupancy information for MID
///

#ifndef MIDRATESCOMPUTERALGORITHM_H
#define MIDRATESCOMPUTERALGORITHM_H

#include <MUONBase/OccupancyMapping.h>
#include <string>
#include "MUONBase/DataStructs.h"

namespace o2
{
namespace muon
{
namespace mid
{
class MIDRatesComputerAlgorithm
{
 public:
  MIDRatesComputerAlgorithm() : fCounter(0){};
  virtual ~MIDRatesComputerAlgorithm();

  bool Init(std::string mappingFileName);
  bool Exec(std::vector<uint32_t> data);
  std::shared_ptr<std::vector<uint64_t>> Output();

 private:
  OccupancyMapping fMapping;
  stripMask fStructMaskSim;
  long fCounter;

  void ResetCounters(uint64_t newStartTS, digitType type);
  bool ShouldComputeRates(digitType type);
  void ComputeRate(stripMapping* strip);
  void ComputeAllRates();
};
} // namespace mid
} // namespace muon
} // namespace o2
#endif // MIDRATESCOMPUTERALGORITHM_H
