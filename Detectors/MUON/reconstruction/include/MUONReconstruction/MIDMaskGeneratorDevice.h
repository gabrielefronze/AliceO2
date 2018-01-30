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

#ifndef O2_DEV_ALO_MIDMASKGENERATOR_H
#define O2_DEV_ALO_MIDMASKGENERATOR_H

#include <dtrace.h>
#include "FairMQDevice.h"
#include "MUONBase/Enums.h"
#include "MUONBase/OccupancyMapping.h"

namespace o2
{
namespace muon
{
namespace mid
{
class MIDMaskGeneratorDevice //: public FairMQDevice
{
 public:
  MIDMaskGeneratorDevice();

  virtual ~MIDMaskGeneratorDevice();

 protected:
  bool HandleData(FairMQMessagePtr&, int);

  virtual void InitTask();

 private:
  OccupancyMapping fMapping;

  using IDType = uint32_t;

  struct stripMask {
    ushort_t nDead;                            // number of elements for deadStripsIDs
    ushort_t nNoisy;                           // number of elements for noisyStripsIDs
    std::unordered_set<IDType> deadStripsIDs;  // container of UniqueIDs of dead strips
    std::unordered_set<IDType> noisyStripsIDs; // container of UniqueIDs of noisy strips
  };

  stripMask fStructMask;
  stripMask fStructMaskSim;

  void FindNoisy(digitType type);

  void FindDead(digitType type);

  void ResetAll();

  void FillMask();

  errMsg SendMask();
};
} // namespace mid
} // namespace muon
} // namespace o2

#endif // O2_DEV_ALO_MIDMASKGENERATOR_H
