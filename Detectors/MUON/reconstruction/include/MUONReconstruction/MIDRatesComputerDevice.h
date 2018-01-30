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
///  @file   MIDRatesComputerDevice
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Device to launch MIDRatesComputerAlgorithm
///

#ifndef MIDRATESCOMPUTER_H
#define MIDRATESCOMPUTER_H

//#include <fairmq/FairMQDevice.h>
//#include "FairMQDevice.h"
#include "MUONBase/Chrono.h"
#include "MUONReconstruction/MIDRatesComputerAlgorithm.h"

namespace o2
{
namespace muon
{
namespace mid
{
class MIDRatesComputerDevice : public FairMQDevice
{
 public:
  MIDRatesComputerDevice();

 protected:
  virtual void InitTask();
  bool HandleData(FairMQMessagePtr&, int);

 private:
  MIDRatesComputerAlgorithm fAlgorithm;
  template <typename T>
  errMsg SendRates(std::shared_ptr<std::vector<T>> digitsOut);

  // Chrono object to compute duration
  Chrono fChronometerHandleData;
  Chrono fChronometerSendData;
};
} // namespace mid
} // namespace muon
} // namespace o2

#endif // MIDRATESCOMPUTER_H
