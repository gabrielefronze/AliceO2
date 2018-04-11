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

#include "MIDFilterAlgorithm.h"

namespace o2
{
namespace mid
{

MIDFilterAlgorithm::MIDFilterAlgorithm() { MIDFilterAlgorithm::Init(); }

bool MIDFilterAlgorithm::Init()
{
  fMask.nDead = 0;
  fMask.nNoisy = 0;
  fMask.deadStripsIDs.clear();
  fMask.noisyStripsIDs.clear();
}

bool MIDFilterAlgorithm::ExecFilter(std::vector<uint32_t> data)
{
  auto returnValue = false;

  // Check if no noisy strip is found. If none simply forward the message
  if (fMask.nNoisy == 0) {
    return returnValue;
  } else {
    for (auto& itData : data) {
      // If the strip is not found in the mask IsStripOk is true
      auto IsStripOk = (fMask.noisyStripsIDs.find(itData) == fMask.noisyStripsIDs.end());

      // If no digit is edited returnValue must be false: in that case the forward is easier
      returnValue |= IsStripOk;

      // Masking the noisy strips in the OpenCL way! We are ready for further optimization...
      itData *= IsStripOk;
    }
  }

  return returnValue;
}

bool MIDFilterAlgorithm::ExecMaskLoading(unsigned short* maskHeader, uint32_t* maskData)
{
  // Clearing the mask data. The new mask is a complete information (not a diff).
  Init();

  // If the received message has no problematic strip just leave the mask empty
  if (maskHeader[0] == 0 && maskHeader[1] == 0) {
    return true;
  }

  // Load the number of dead and noisy strips in the mask
  fMask.nDead = maskHeader[0];
  fMask.nNoisy = maskHeader[1];

  // Load the unique IDs in the mask object
  if (fMask.nDead > 0)
    fMask.deadStripsIDs = std::unordered_set<uint32_t>(&(maskData[0]), &(maskData[fMask.nDead - 1]));
  if (fMask.nNoisy > 0)
    fMask.noisyStripsIDs =
      std::unordered_set<uint32_t>(&(maskData[fMask.nDead]), &(maskData[fMask.nDead + fMask.nNoisy - 1]));

  return true;
}

} // namespace mid
} // namespace o2
