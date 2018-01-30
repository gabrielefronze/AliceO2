///
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

#include <FairLogger.h>
#include "MUONReconstruction/MIDFilterAlgorithm.h"

using namespace o2::muon::mid;

MIDFilterAlgorithm::MIDFilterAlgorithm()
{
  MIDFilterAlgorithm::Init();
}

bool MIDFilterAlgorithm::Init()
{
  fMask.nDead = 0;
  fMask.nNoisy = 0;
  fMask.deadStripsIDs.clear();
  fMask.noisyStripsIDs.clear();
}

bool MIDFilterAlgorithm::ExecFilter(std::vector<uint32_t> data)
{
  // Check if no noisy strip is found. If none simply forward the message
  if (fMask.nNoisy == 0) {
    return false;
  }

  for ( auto &itData : data ){
    // Masking the noisy strips in the OpenCL way!
    auto IsStripOk = fMask.noisyStripsIDs.find(itData) == fMask.noisyStripsIDs.end();
    itData *= IsStripOk;
  }

  return true;
}

bool MIDFilterAlgorithm::ExecMaskLoading(unsigned short counters[2], std::vector<uint32_t> maskData)
{

  // Clearing the mask data. The new mask is a complete information (not a diff).
  Init();

  // If the received message has no problematic strip just leave the mask empty
  if (counters[0] == 0 && counters[1] == 0) {
    LOG(DEBUG) << "Received empty mask.";
    return true;
  }

  // Load the number of dead and noisy strips in the mask
  fMask.nDead = counters[0];
  fMask.nNoisy = counters[1];

  // Load the unique IDs in the mask object
  if (fMask.nDead > 0)
    fMask.deadStripsIDs = std::unordered_set<uint32_t>(&(maskData[0]), &(maskData[fMask.nDead - 1]));
  if (fMask.nNoisy > 0)
    fMask.noisyStripsIDs =
      std::unordered_set<uint32_t>(&(maskData[fMask.nDead]), &(maskData[fMask.nDead + fMask.nNoisy - 1]));

  //    LOG(DEBUG) << "Mask correctly loaded with " << fMask.nDead + fMask.nNoisy << " problematic strips";

  return true;
}
