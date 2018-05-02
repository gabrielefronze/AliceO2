// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDSimulation/Digitizer.h
/// \brief  Digitizer for MID
/// \author Diego Stocco <Diego.Stocco at cern.ch>
/// \date   01 March 2018
#ifndef O2_MID_DIGITIZER_H
#define O2_MID_DIGITIZER_H

#include <random>
#include <vector>
#include <array>
#include "MathUtils/Cartesian3D.h"
#include "DataFormatsMID/ColumnData.h"
#include "MIDBase/Mapping.h"
#include "MIDSimulation/ChamberResponse.h"

namespace o2
{
namespace mid
{
class Digitizer
{
 public:
  Digitizer();
  virtual ~Digitizer() = default;

  Digitizer(const Digitizer&) = delete;
  Digitizer& operator=(const Digitizer&) = delete;
  Digitizer(Digitizer&&) = delete;
  Digitizer& operator=(Digitizer&&) = delete;

  std::vector<ColumnData> hitToDigits(const Point3D<double>& hit, int deId) const;

 private:
  void addStrip(const Mapping::MpStripIndex& stripIndex, int cathode, int deId) const;
  bool addStrips(double xPos, double yPos, int cathode, int deId, double prob, double* xDist1 = nullptr,
                 double* xDist2 = nullptr, double xOffset = 0.) const;
  bool addNeighbours(const Mapping::MpStripIndex& stripIndex, int cathode, int deId, double prob,
                     const std::array<double, 2>& initialDist, double xOffset) const;
  mutable std::default_random_engine mGenerator;          ///< Random numbers generator
  mutable std::uniform_real_distribution<double> mRandom; ///< Uniform distribution
  ChamberResponse mResponse;                              ///< Chamber response
  Mapping mMapping;                                       ///< Mapping
  mutable std::vector<ColumnData> mDigits;                ///< Array of digits
};
} // namespace mid
} // namespace o2

#endif /* O2_MID_DIGITIZER_H */
