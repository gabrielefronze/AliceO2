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

using namespace o2::muon::mid;

#include "MUONReconstruction/MIDMaskGeneratorAlgorithm.h"
#include <fairmq/logger/logger.h>
#include <numeric>
#include "MUONBase/Enums.h"

using namespace o2::muon::mid;

bool MIDMaskGeneratorAlgorithm::Init(std::string mappingFileName)
{
  if (!(fMapping.ReadMapping(mappingFileName.c_str()))) {
    LOG(ERROR) << "Error reading the mapping from " << mappingFileName;
  }

  return fMapping.Consistent(true);
}

MIDMaskGeneratorAlgorithm::~MIDMaskGeneratorAlgorithm()
{
  LOG(DEBUG) << "Detected noisy strips:";
  for (const auto& itMask : fStructMask.noisyStripsIDs) {
    LOG(DEBUG) << "\t" << itMask << "\t\t" << fMapping[itMask]->digitsCounter[digitType::kPhysics];
  }
}

bool MIDMaskGeneratorAlgorithm::Exec(std::vector<uint64_t> data)
{
  // Copy the payload of the message in the internal data container
  size_t iData = 0;
  for (const auto& itData : data) {
    fMapping.fStripVector[iData].digitsCounter[iData % digitsType::kSize] = itData;
    iData++;
  }

  LOG(DEBUG) << "Message parsing done!";

  FindNoisy(digitType::kFET);
  FindDead(digitType::kTriggered);

  FillMask();
}

void MIDMaskGeneratorAlgorithm::FindNoisy(digitType type)
{
  std::for_each(fMapping.fStripVector.begin(), fMapping.fStripVector.end(), [type](stripMapping strip) {
    if (strip.digitsCounter[type] != 0)
      strip.isNoisy = true;
  });
}

void MIDMaskGeneratorAlgorithm::FindDead(digitType type)
{
  std::for_each(fMapping.fStripVector.begin(), fMapping.fStripVector.end(), [type](stripMapping strip) {
    if (strip.digitsCounter[type] == 0)
      strip.isDead = true;
  });
}

void MIDMaskGeneratorAlgorithm::FillMask()
{
  for (const auto& mapIterator : fMapping.fIDMap) {
    auto uniqueID = mapIterator.first;
    auto index = mapIterator.second;
    auto strip = &(fMapping.fStripVector[index]);

    if (strip->isDead) {
      auto alreadyThere = fStructMask.deadStripsIDs.insert(uniqueID).second;

      if (alreadyThere) {
        LOG(ERROR) << uniqueID << " is dead.";
      } else {
        //                LOG(INFO)<<uniqueID<<" already set.";
      }

    } else if (strip->isNoisy) {
      auto alreadyThere = fStructMask.noisyStripsIDs.insert(uniqueID).second;

      if (alreadyThere) {
        LOG(ERROR) << uniqueID << " is noisy.";
      } else {
        //                LOG(INFO)<<uniqueID<<" already set.";
      }
    }
    //        else LOG(INFO)<<uniqueID<<" is working as expected.";
  }

  fStructMask.nDead = (ushort_t)fStructMask.deadStripsIDs.size();
  fStructMask.nNoisy = (ushort_t)fStructMask.noisyStripsIDs.size();

  return;
}
