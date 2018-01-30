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

#ifndef MIDFILTERDEVICE_H
#define MIDFILTERDEVICE_H

//#include "FairMQDevice.h"
#include "MUONBase/Chrono.h"
#include "MUONReconstruction/MIDFilterAlgorithm.h"

namespace o2
{
namespace muon
{
namespace mid
{
class MIDFilterDevice : public FairMQDevice
{
 public:
  MIDFilterDevice();

 protected:
  bool HandleData(FairMQMessagePtr&, int);
  bool HandleMask(FairMQMessagePtr&, int);

 private:
  MIDFilterAlgorithm fAlgorithm;
  template <typename T>
  errMsg SendMsg(uint64_t msgSize, T* data);

  // Chrono object to compute duration
  Chrono fChronometer;
};
} // namespace mid
} // namespace muon
} // namespace o2

#endif // MIDFILTERDEVICE_H