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
#include <numeric>
#include "FairMQLogger.h"
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
    fMapping.fStripVector[iData].digitsCounter[iData % digitType::kSize] = itData;
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
      auto alreadyThere = fMask.deadStripsIDs.insert(uniqueID).second;

      if (alreadyThere) {
        LOG(ERROR) << uniqueID << " is dead.";
      } else {
        //                LOG(INFO)<<uniqueID<<" already set.";
      }

    } else if (strip->isNoisy) {
      auto alreadyThere = fMask.noisyStripsIDs.insert(uniqueID).second;

      if (alreadyThere) {
        LOG(ERROR) << uniqueID << " is noisy.";
      } else {
        //                LOG(INFO)<<uniqueID<<" already set.";
      }
    }
    //        else LOG(INFO)<<uniqueID<<" is working as expected.";
  }

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
  auto header = reinterpret_cast<unsigned short*>(&outputVect[0]);
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