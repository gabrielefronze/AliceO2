// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDSimulation/ChamberResponse.h
/// \brief  MID RPC response
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   30 April 2018

#ifndef O2_MID_CHAMBERRESPONSE_H
#define O2_MID_CHAMBERRESPONSE_H

#include "MIDSimulation/ChamberResponseParams.h"
#include "MIDSimulation/ChamberHV.h"

namespace o2
{
namespace mid
{
class ChamberResponse
{
 public:
  ChamberResponse();
  virtual ~ChamberResponse() = default;

  ChamberResponse(const ChamberResponse&) = delete;
  ChamberResponse& operator=(const ChamberResponse&) = delete;
  ChamberResponse(ChamberResponse&&) = delete;
  ChamberResponse& operator=(ChamberResponse&&) = delete;

  inline bool isFired(double prob, double distance, int cathode, int deId, double theta = 0.) const
  {
    /// Check if the strip at a certain distance from the impact point is fired
    /// given a probability prob.
    return (prob < getFiredProbability(distance, cathode, deId, theta));
  }
  double getFiredProbability(double distance, int cathode, int deId, double theta = 0.) const;

 private:
  ChamberResponseParams mParams; ///< Chamber response parameters
  ChamberHV mHV;                 ///< HV values for chambers
};
} // namespace mid
} // namespace o2

#endif /* O2_MID_CHAMBERRESPONSE_H */
