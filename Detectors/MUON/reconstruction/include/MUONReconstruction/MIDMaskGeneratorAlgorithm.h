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
///  @file   MIDMaskGeneratorAlgorithm
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Algorithm to compute mask based on computed rates for MID
///

#ifndef MIDMASKGENERATORALGORITHM_H
#define MIDMASKGENERATORALGORITHM_H

#include <MUONBase/DataStructs.h>
#include <MUONBase/OccupancyMapping.h>
#include <string>
#include <vector>

namespace o2
{
namespace muon
{
namespace mid
{
class MIDMaskGeneratorAlgorithm
{
 public:
  MIDMaskGeneratorAlgorithm() = default;

  virtual ~MIDMaskGeneratorAlgorithm();

  bool Init(std::string mappingFileName);
  bool Exec(uint64_t* data);
  inline stripMask* GetMask(){ return &fMask; };

 private:
  OccupancyMapping fMapping;
  stripMask fMask;

  void FindNoisy(digitType type = kTriggered);
  void FindDead(digitType type = kFET);
  void ResetAll();
  void FillMask();
};
} // namespace mid
} // namespace muon
} // namespace o2

#endif // MIDMASKGENERATORALGORITHM_H
