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

  MIDMaskGeneratorAlgorithm::FindNoisy(digitType::kFET);
  MIDMaskGeneratorAlgorithm::FindDead(digitType::kTriggered);

  MIDMaskGeneratorAlgorithm::FillMask();
}

void MIDMaskGeneratorAlgorithm::FindNoisy(digitType type)
{
  std::for_each(fMapping.fStripVector.begin(),fMapping.fStripVector.end(),[type](auto strip){strip.});
}
