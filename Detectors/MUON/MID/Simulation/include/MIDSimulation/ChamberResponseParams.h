// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDSimulation/ChamberResponseParams.h
/// \brief  Parameters for MID RPC response
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   26 April 2018
#ifndef O2_MID_CHAMBERRESPONSEPARAMS_H
#define O2_MID_CHAMBERRESPONSEPARAMS_H

#include <array>

namespace o2
{
namespace mid
{
class ChamberResponseParams
{
 public:
  ChamberResponseParams();
  virtual ~ChamberResponseParams() = default;

  ChamberResponseParams(const ChamberResponseParams&) = delete;
  ChamberResponseParams& operator=(const ChamberResponseParams&) = delete;
  ChamberResponseParams(ChamberResponseParams&&) = delete;
  ChamberResponseParams& operator=(ChamberResponseParams&&) = delete;

  double getParA(double hv) const;
  double getParB(int cathode, int deId) const;
  double getParC(double hv) const;

  void setDefaultParams(bool isStreamer = false);

 private:
  void setDefaultParB(bool isStreamer);
  std::array<double, 2> mParA;   ///< Values to compute first parameter
  std::array<double, 2> mParC;   ///< Values to compute third parameter
  std::array<double, 144> mParB; ///< Array of second parameter
};
} // namespace mid
} // namespace o2

#endif /* O2_MID_CHAMBERRESPONSEPARAMS_H */
