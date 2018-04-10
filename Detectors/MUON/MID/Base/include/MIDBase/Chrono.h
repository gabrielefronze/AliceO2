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

#ifndef CHRONO_H
#define CHRONO_H

#include <chrono>
#include <cstdint>
#include <numeric>
#include <ratio>
#include <sstream>
#include <string>
#include <vector>

namespace o2
{
namespace muon
{
namespace mid
{
class Chrono
{
 public:
  void AddCall(double deltaT) { fDeltaTs.emplace_back(deltaT); };

  double AvgCallTime() { return std::accumulate(fDeltaTs.begin(), fDeltaTs.end(), 0.) / fDeltaTs.size(); };
  inline size_t GetNCalls() { return fDeltaTs.size(); };
  std::string PrintStatus()
  {
    std::ostringstream outputString;
    outputString << "NCalls=" << std::to_string(fDeltaTs.size()) << " AvgTime=";
    outputString << AvgCallTime();
    return outputString.str();
  };

 private:
  std::vector<double> fDeltaTs;
};

class DeltaT
{
 public:
  DeltaT(Chrono* chrono) : fStart(std::chrono::high_resolution_clock::now()), fChrono(chrono){};
  ~DeltaT()
  {
    fChrono->AddCall(
      std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - fStart).count());
  };

 private:
  std::chrono::high_resolution_clock::time_point fStart;
  Chrono* fChrono;
};
} // namespace mid
} // namespace muon
} // namespace o2

#endif // CHRONO_H
