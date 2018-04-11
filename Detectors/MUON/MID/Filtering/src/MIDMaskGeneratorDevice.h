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
#include "MIDBase/Enums.h"
#include "MIDBase/OccupancyMapping.h"
#include "MIDMaskGeneratorAlgorithm.h"

namespace o2
{
namespace mid
{
class MIDMaskGeneratorDevice : public FairMQDevice
{
 public:
  MIDMaskGeneratorDevice();
  virtual ~MIDMaskGeneratorDevice();

 protected:
  bool HandleData(FairMQMessagePtr&, int);
  virtual void InitTask();

 private:
  MIDMaskGeneratorAlgorithm fAlgorithm;
  errMsg SendMask();
};
} // namespace mid
} // namespace o2

#endif // O2_DEV_ALO_MIDMASKGENERATOR_H
