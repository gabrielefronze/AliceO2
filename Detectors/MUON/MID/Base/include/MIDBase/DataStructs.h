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
///  @file   DataStructs
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Container for usefull data structs
///

#ifndef O2_MID_DATASTRUCTS_H
#define O2_MID_DATASTRUCTS_H

#include <math.h>
#include <sys/dtrace.h>
#include <unordered_set>
#include "MIDBase/Enums.h"

namespace o2
{
namespace mid
{
// Human readable data struct for output
struct deserializerDataStruct {
  uint32_t fDetElemID;
  uint32_t fBoardID;
  uint32_t fChannel;
  uint32_t fCathode;

  deserializerDataStruct(uint32_t detElemID = 0, uint32_t boardID = 0, uint32_t channel = 0, uint32_t cathode = 0)
  {
    fDetElemID = detElemID;
    fBoardID = boardID;
    fChannel = channel;
    fCathode = cathode;
  }
};

struct stripMapping {
  uint64_t startTS[digitType::kSize];       // timestamp of first added run
  uint64_t stopTS[digitType::kSize];        // timestamp of last added run
  uint64_t digitsCounter[digitType::kSize]; // counter of time the strip has been fired
  ushort_t columnID;
  float_t area; // 1D area
  float_t coord[2][2];
  float_t rate[digitType::kSize]; // rate in Hz/cm2 or 1/cm2
  bool isDead;
  bool isNoisy;

  stripMapping();
};

struct stripMask {
  ushort_t nDead;                              // number of elements for deadStripsIDs
  ushort_t nNoisy;                             // number of elements for noisyStripsIDs
  std::unordered_set<uint32_t> deadStripsIDs;  // container of UniqueIDs of dead strips
  std::unordered_set<uint32_t> noisyStripsIDs; // container of UniqueIDs of noisy strips
};

} // namespace mid
} // namespace o2

#endif // O2_MID_DATASTRUCTS_H
