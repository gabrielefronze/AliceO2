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

#include "MIDMaskGeneratorAlgorithm.h"
#include <iostream>
#include <numeric>
#include "FairMQLogger.h"
#include "MIDBase/Enums.h"

namespace o2
{
namespace mid
{

//_________________________________________________________________________________________________
bool MIDMaskGeneratorAlgorithm::Init(std::string mappingFileName)
{
  if (!(fMapping.ReadMapping(mappingFileName.c_str()))) {
    std::cout << "Error reading the mapping from " << mappingFileName;
  }

  return fMapping.Consistent();
}

//_________________________________________________________________________________________________
MIDMaskGeneratorAlgorithm::~MIDMaskGeneratorAlgorithm()
{
  std::cout << "Detected noisy strips:";
  for (const auto& itMask : fMask.noisyStripsIDs) {
    std::cout << "\t" << itMask << "\t\t" << fMapping[itMask]->digitsCounter[digitType::kPhysics];
  }
}

//_________________________________________________________________________________________________
bool MIDMaskGeneratorAlgorithm::Exec(uint64_t* data)
{
  // Copy the payload of the message in the internal data container
  for (int iData = 0; iData < fMapping.fStripVector.size(); iData++) {
    for (int iType = 0; iType < digitType::kSize; iType++) {
      fMapping.fStripVector[iData].digitsCounter[iType] = data[iData * 3 + iType];
    }
  }

  // Calling the FindNoisy and FindDead method to set the flags of the fMapping elements
  FindNoisy(digitType::kFET);
  FindDead(digitType::kTriggered);

  // Filling the mask
  FillMask();

  return true;
}

//_________________________________________________________________________________________________
void MIDMaskGeneratorAlgorithm::FindNoisy(digitType type)
{
  std::for_each(fMapping.fStripVector.begin(), fMapping.fStripVector.end(), [type](stripMapping strip) {
    if (strip.digitsCounter[type] != 0)
      strip.isNoisy = true;
  });
}

//_________________________________________________________________________________________________
void MIDMaskGeneratorAlgorithm::FindDead(digitType type)
{
  std::for_each(fMapping.fStripVector.begin(), fMapping.fStripVector.end(), [type](stripMapping strip) {
    if (strip.digitsCounter[type] == 0)
      strip.isDead = true;
  });
}

//_________________________________________________________________________________________________
void MIDMaskGeneratorAlgorithm::FillMask()
{
  for (const auto& mapIterator : fMapping.fIDMap) {
    auto uniqueID = mapIterator.first;
    auto index = mapIterator.second;
    auto strip = &(fMapping.fStripVector[index]);

    bool alreadyThere = false;
    if (strip->isDead) {
      alreadyThere = fMask.deadStripsIDs.insert(uniqueID).second;
    } else if (strip->isNoisy) {
      alreadyThere = fMask.noisyStripsIDs.insert(uniqueID).second;
    }
  }

  // Populating fMask header
  fMask.nDead = (ushort_t)fMask.deadStripsIDs.size();
  fMask.nNoisy = (ushort_t)fMask.noisyStripsIDs.size();

  return;
}

} // namespace mid
} // namespace o2