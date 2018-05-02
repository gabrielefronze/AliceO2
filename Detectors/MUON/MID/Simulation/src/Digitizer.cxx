// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Simulation/src/Digitizer.cxx
/// \brief  Implementation of the digitizer for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   01 March 2018
#include "MIDSimulation/Digitizer.h"

namespace o2
{
namespace mid
{
//______________________________________________________________________________
Digitizer::Digitizer() : mGenerator(std::default_random_engine()), mRandom(), mResponse(), mMapping(), mDigits()
{
  /// Default constructor
}

//______________________________________________________________________________
void Digitizer::addStrip(const Mapping::MpStripIndex& stripIndex, int cathode, int deId) const
{
  /// Adds strip
  // std::cout << "Add cath: " << cathode << "  col: " << stripIndex.column << "  line: " << stripIndex.line << "  strip: " << stripIndex.strip << std::endl; // REMEMBER TO CUT
  for (auto& col : mDigits) {
    if (col.deId != deId) {
      continue;
    }
    if (col.columnId != stripIndex.column) {
      continue;
    }
    col.addStrip(stripIndex.strip, cathode, stripIndex.line);
    return;
  }

  mDigits.emplace_back(ColumnData{ (uint8_t)deId, (uint8_t)stripIndex.column });
  mDigits.back().addStrip(stripIndex.strip, cathode, stripIndex.line);
}

//______________________________________________________________________________
bool Digitizer::addStrips(double xPos, double yPos, int cathode, int deId, double prob, double* xDist1, double* xDist2,
                          double xOffset) const
{
  /// Add current strip

  // Warn only on the first point which is supposed to be inside the RPC
  // Do not warn when we attempt to check the neighbour columns
  // by setting an offset along x
  bool warn = (xOffset == 0.);
  Mapping::MpStripIndex stripIndex = mMapping.stripByPosition(xPos + 1.01 * xOffset, yPos, cathode, deId, warn);
  if (!stripIndex.isValid()) {
    return false;
  }

  if (xOffset != 0. && !mResponse.isFired(prob, std::abs(xOffset), cathode, deId)) {
    return false;
  }
  // std::cout << "Fired strip (cath " << cathode << "): (" << stripIndex.column << "," << stripIndex.line << "," << stripIndex.strip << ")  dist: " << std::abs(xOffset) << "  prob: " << prob << " < " << mResponse.getFiredProbability(std::sqrt(xOffset * xOffset), cathode, deId) << std::endl; // REMEMBER TO CUT
  addStrip(stripIndex, cathode, deId);
  MpArea area = mMapping.stripByLocation(stripIndex.strip, cathode, stripIndex.line, stripIndex.column, deId);
  std::array<double, 2> dist;
  dist[0] = (cathode == 0) ? yPos - area.getYmin() : xPos - area.getXmin();
  dist[1] = (cathode == 0) ? area.getYmax() - yPos : area.getXmax() - xPos;
  if (xDist1) {
    *xDist1 = area.getXmin() - xPos;
  }
  if (xDist2) {
    *xDist2 = area.getXmax() - xPos;
  }
  addNeighbours(stripIndex, cathode, deId, prob, dist, xOffset);
  return true;
}

//______________________________________________________________________________
bool Digitizer::addNeighbours(const Mapping::MpStripIndex& stripIndex, int cathode, int deId, double prob,
                              const std::array<double, 2>& initialDist, double xOffset) const
{
  /// Add neighbour strips
  double xOffset2 = xOffset * xOffset;
  for (int idir = 0; idir < 2; ++idir) {
    double dist = initialDist[1 - idir];
    Mapping::MpStripIndex neigh = mMapping.nextStrip(stripIndex, cathode, deId, idir);
    // std::cout << "Strip: (" << neigh.column << "," << neigh.line << "," << neigh.strip << ")  dist:  " << std::sqrt(dist * dist + xOffset2) << "  fired = (" << prob << " < " << mResponse.getFiredProbability(std::sqrt(dist * dist + xOffset2), cathode, deId) << ")" << std::endl; // REMEMBER TO CUT
    while (neigh.isValid() && mResponse.isFired(prob, std::sqrt(dist * dist + xOffset2), cathode, deId)) {
      // std::cout << "Prob " << prob << "  fireProb(" << dist << ", " << xOffset << ") "
      //           << mResponse.getFiredProbability(std::sqrt(dist * dist + xOffset2), cathode, deId)
      //           << std::endl; // REMEMBER TO CUT
      addStrip(neigh, cathode, deId);
      dist += mMapping.getStripSize(neigh.strip, cathode, neigh.column, deId);
      neigh = mMapping.nextStrip(neigh, cathode, deId, idir);
      // std::cout << "Strip: (" << neigh.column << "," << neigh.line << "," << neigh.strip << ")  dist:  " << std::sqrt(dist * dist + xOffset2) << "  fired = (" << prob << " < " << mResponse.getFiredProbability(std::sqrt(dist * dist + xOffset2), cathode, deId) << ")" << std::endl; // REMEMBER TO CUT
    }
  }
  return true;
}

//______________________________________________________________________________
std::vector<ColumnData> Digitizer::hitToDigits(const Point3D<double>& hit, int deId) const
{
  /// Generate digits from the hit
  mDigits.clear();

  // Check NBP
  double prob = mRandom(mGenerator);
  if (addStrips(hit.x(), hit.y(), 1, deId, prob)) {
    // If the hit does not touch the strips: no need to go further

    double xDist1, xDist2;
    // // Here we're assuming a different probability for the BP and NBP
    // // This is how was implemented in aliroot
    // prob = mRandom(mGenerator);
    // Check BP
    addStrips(hit.x(), hit.y(), 0, deId, prob, &xDist1, &xDist2);
    // Inner neighbours:
    addStrips(hit.x(), hit.y(), 0, deId, prob, nullptr, nullptr, xDist1);
    // Outer neighbours
    addStrips(hit.x(), hit.y(), 0, deId, prob, nullptr, nullptr, xDist2);
  }

  // for (int icath = 0; icath < 2; ++icath) {
  //   Mapping::MpStripIndex stripIndex = mMapping.stripByPosition(hit.x(), hit.y(), icath, deId);
  //   if (!stripIndex.isValid()) {
  //     break;
  //   }
  //   addStrip(stripIndex, icath, deId, digits);
  //   MpArea area = mMapping.stripByLocation(stripIndex.strip, icath, stripIndex.line, stripIndex.column, deId);
  //   double prob = mRandom(mGenerator);
  //   const double pos[4] = { area.getXmin(), area.getXmax(), area.getYmin(), area.getYmax() };
  //   for (int idir = 0; idir < 2; ++idir) {
  //     double dist = (icath == 0) ? hit.y() - pos[3 - idir] : hit.x() - pos[1 - idir];
  //     Mapping::MpStripIndex neigh = mMapping.nextStrip(stripIndex, icath, deId, idir);
  //     while (neigh.isValid() || mResponse.isFired(prob, dist, deId, icath)) {
  //       addStrip(neigh, icath, deId, digits);
  //       dist += mMapping.getStripSize(neigh.strip, icath, neigh.column, deId);
  //       neigh = mMapping.nextStrip(neigh, icath, deId, idir);
  //     }
  //   }
  // }
  return mDigits;
}

} // namespace mid
} // namespace o2
