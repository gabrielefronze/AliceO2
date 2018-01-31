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

#include "MUONReconstruction/MIDMaskGeneratorAlgorithm.h"
#include <iostream>
#include <numeric>
#include "FairMQLogger.h"
#include "MUONBase/Enums.h"

using namespace o2::muon::mid;

bool MIDMaskGeneratorAlgorithm::Init(std::string mappingFileName)
{
  if (!(fMapping.ReadMapping(mappingFileName.c_str()))) {
    std::cout << "Error reading the mapping from " << mappingFileName;
  }

  return fMapping.Consistent(true);
}

MIDMaskGeneratorAlgorithm::~MIDMaskGeneratorAlgorithm()
{
  std::cout << "Detected noisy strips:";
  for (const auto& itMask : fMask.noisyStripsIDs) {
    std::cout << "\t" << itMask << "\t\t" << fMapping[itMask]->digitsCounter[digitType::kPhysics];
  }
}

bool MIDMaskGeneratorAlgorithm::Exec(std::vector<uint64_t> data)
{
  // Copy the payload of the message in the internal data container
  size_t iData = 0;
  for (const auto& itData : data) {
    fMapping.fStripVector[iData].digitsCounter[iData % digitType::kSize] = itData;
    iData++;
  }

  // Calling the FindNoisy and FindDead method to set the flags of the fMapping elements
  FindNoisy(digitType::kFET);
  FindDead(digitType::kTriggered);

  // Filling the mask
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

std::shared_ptr<std::vector<uint32_t>> MIDMaskGeneratorAlgorithm::Output()
{
  // Computing how many UID have to be sent
  auto sum = fMask.nDead + fMask.nNoisy;

  // Allocating container
  std::vector<uint32_t> outputVect(sum + 1);

  // Loading the header info
  auto header = reinterpret_cast<ushort_t*>(&outputVect[0]);
  header[0] = fMask.nDead;
  header[1] = fMask.nNoisy;

  // Loading the UIDs in the output vector
  int position = 1;
  for (auto const& itDead : fMask.deadStripsIDs) {
    outputVect[position++] = itDead;
  }
  for (auto const& itNoisy : fMask.noisyStripsIDs) {
    outputVect[position++] = itNoisy;
  }

  // returning a shared pointer to the vector
  return std::make_shared(outputVect);
}