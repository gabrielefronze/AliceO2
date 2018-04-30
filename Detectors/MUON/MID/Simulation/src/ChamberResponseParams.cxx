// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Simulation/src/ChamberResponseParams.cxx
/// \brief  Implementation of the parameters for MID RPC response
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   26 April 2018

/// This class implements the parameters for the parameterization of the RPC spatial resolution.
/// The parameters were tuned by Massimiliano Marchisone in his PhD thesis:
/// http://www.theses.fr/2013CLF22406
/// See ChamberResponse for further details

#include "MIDSimulation/ChamberResponseParams.h"

namespace o2
{
namespace mid
{
//______________________________________________________________________________
ChamberResponseParams::ChamberResponseParams() : mParA(), mParC(), mParB()
{
  /// Default constructor
}

//______________________________________________________________________________
void ChamberResponseParams::setDefaultParams(bool isStreamer)
{
  /// Sets the default parameters
  mParA[0] = -52.70;
  mParA[1] = 6.089 / 1000.; // 1/V
  mParC[0] = -0.5e-3;
  mParC[1] = 8.3e-4 / 1000.; // 1/V
  setDefaultParB(isStreamer);
}

//______________________________________________________________________________
void ChamberResponseParams::setDefaultParB(bool isStreamer)
{ /// Sets the default parameters
  if (isStreamer) {
    mParB.fill(2.966);
    return;
  }

  mParB = { {
    // BP
    2.97, 2.47, 2.47, 1.97, 1.97, 2.47, 2.47, 2.47, 2.97, // MT11R
    2.97, 1.97, 1.97, 1.97, 2.22, 2.22, 1.97, 2.47, 2.97, // MT12R
    2.97, 1.97, 1.97, 1.97, 2.22, 2.22, 2.47, 2.47, 2.97, // MT21R
    2.97, 1.97, 1.97, 1.97, 1.97, 1.97, 2.97, 2.97, 2.97, // MT22R
    2.97, 1.97, 2.47, 1.97, 2.22, 1.97, 2.47, 2.47, 2.97, // MT11L
    2.97, 1.97, 2.47, 1.97, 1.97, 1.97, 2.47, 1.97, 2.97, // MT12L
    2.97, 1.97, 2.47, 1.97, 1.97, 2.22, 2.47, 2.47, 2.97, // MT21L
    2.97, 2.22, 2.47, 1.72, 1.97, 1.97, 1.97, 2.47, 2.97, // MT22L
    // NBP
    2.97, 2.97, 1.97, 1.72, 1.97, 2.47, 2.47, 2.97, 2.97, // MT11R
    2.97, 2.97, 1.97, 1.97, 2.47, 1.97, 2.22, 2.97, 2.97, // MT12R
    2.97, 2.47, 1.97, 1.97, 1.97, 2.47, 2.47, 2.97, 2.97, // MT21R
    2.97, 1.97, 1.97, 1.97, 1.72, 1.97, 2.97, 2.97, 2.97, // MT22R
    2.97, 2.97, 2.47, 2.22, 1.97, 1.97, 2.47, 2.97, 2.97, // MT11L
    2.97, 2.97, 2.97, 1.97, 1.97, 1.97, 2.97, 2.47, 2.97, // MT12L
    2.97, 2.97, 2.47, 2.22, 1.97, 2.22, 2.47, 2.97, 2.97, // MT21L
    2.47, 2.97, 2.47, 1.97, 2.22, 1.72, 1.97, 2.97, 2.97  // MT22L
  } };
}

//______________________________________________________________________________
double ChamberResponseParams::getParA(double hv) const
{
  /// Get first parameter
  /// \par hv RPC HV in volts
  return mParA[1] * hv + mParA[0];
}

//______________________________________________________________________________
double ChamberResponseParams::getParC(double hv) const
{
  /// Get third parameter
  /// \par hv RPC HV in volts
  return mParC[1] * hv + mParC[0];
}

//______________________________________________________________________________
double ChamberResponseParams::getParB(int cathode, int deId) const
{
  /// Get second parameter
  return mParB[72 * cathode + deId];
}
} // namespace mid
} // namespace o2
