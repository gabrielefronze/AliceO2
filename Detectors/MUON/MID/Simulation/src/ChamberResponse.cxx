// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Simulation/src/ChamberResponse.cxx
/// \brief  Implementation MID RPC response
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   30 April 2018

/// This class implements the RPC spatial resolution.
/// The original functional form is based on this work:
/// R.~Arnaldi {\it et al.} [ALICE Collaboration],
/// %``Spatial resolution of RPC in streamer mode,''
/// Nucl.\ Instrum.\ Meth.\ A {\bf 490} (2002) 51.
/// doi:10.1016/S0168-9002(02)00917-8
/// The parameters were further tuned by Massimiliano Marchisone in his PhD thesis:
/// http://www.theses.fr/2013CLF22406

#include "MIDSimulation/ChamberResponse.h"

#include <cmath>

namespace o2
{
namespace mid
{
//______________________________________________________________________________
ChamberResponse::ChamberResponse() : mParams(), mHV()
{
  /// Default constructor
  mParams.setDefaultParams();
  mHV.setDefault();
}

//______________________________________________________________________________
double ChamberResponse::getFiredProbability(double distance, int cathode, int deId, double theta) const
{
  /// Get fired probability

  // Need to convert the distance from cm to mm
  double distMM = distance * 10.;
  double parA = mParams.getParA(mHV.getHV(deId));
  double parB = mParams.getParB(cathode, deId);
  double parC = mParams.getParC(mHV.getHV(deId));
  double costheta = std::cos(theta);
  return (parC + parA / (parA + costheta * std::pow(distMM, parB))) / (1 + parC);
}
} // namespace mid
} // namespace o2
