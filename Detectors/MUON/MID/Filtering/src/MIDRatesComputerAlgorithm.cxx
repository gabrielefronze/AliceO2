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

#include <FairMQLogger.h>
#include "MIDRatesComputerAlgorithm.h"
#include "MIDBase/Deserializer.h"
#include <TRandom.h>

namespace o2
{
namespace mid
{

bool MIDRatesComputerAlgorithm::Init(std::string mappingFileName)
{
  fCounter = 0;

  if (!(fMapping.ReadMapping(mappingFileName.c_str()))) {
    LOG(ERROR) << "Error reading the mapping from " << mappingFileName;
  } else {
    LOG(DEBUG) << "Mapping read correctly from " << mappingFileName;
  }

  return fMapping.Consistent(true);
}

MIDRatesComputerAlgorithm::~MIDRatesComputerAlgorithm()
{
  // Output of simulated noisy strips
  LOG(DEBUG) << "Simulated noisy strips:";
  for (const auto& itMask : fStructMaskSim.noisyStripsIDs) {
    LOG(DEBUG) << "\t" << itMask << "\t\t" << fMapping[itMask]->digitsCounter[digitType::kPhysics];
  }
}

bool MIDRatesComputerAlgorithm::Exec(std::vector<uint32_t> data)
{
  fCounter++;

  // Counter of received digits
  int counter = 0;

  LOG(INFO) << "Received valid message with " << data.size() << " digits";

  // Loop over the digits of the message.
  for (const auto& uniqueIDBuffer : data) {

    // We want to discard MCH digits (if any)
    if ((uniqueIDBuffer & 0xFFF) < 1100)
      continue;

    counter++;

    digitType digitType = digitType::kSize;

    // Try to retrieve a pointer to the data member to modify
    stripMapping* strip = fMapping[uniqueIDBuffer];
    if (!strip) {
      //            LOG(ERROR) << "Mapping not found for that uniqueID: " << uniqueIDBuffer;
      continue;
    }

    // TODO: here we need to check the kind of event. No clue on where it is stored for the moment.
    if (true /*physics*/) {
      digitType = digitType::kPhysics;
    } else if (false /*FET*/) {
      digitType = digitType::kFET;
    } else if (false /*Triggered*/) {
      digitType = digitType::kTriggered;
    } else
      continue;

    //            LOG(INFO) << "StripMapping struct found for element: " << MessageDeserializer.PrintData() << " " <<
    //        strip->digitsCounter[0];

    // Increase the counter of digits for the strip
    strip->digitsCounter[digitType]++;
    //        LOG(INFO) << (long int)strip->digitsCounter[digitType];
    if (gRandom->Rndm() > 0.10) {
      strip->digitsCounter[digitType] += 999999;
      //      LOG(INFO) << "Simulating noisy strip " << ((uniqueIDBuffer) & 0xFFF);
      auto dummy = fStructMaskSim.noisyStripsIDs.insert(uniqueIDBuffer).second;
      fStructMaskSim.nNoisy++;
    }
  }

  //    LOG(INFO) << "Computing rates.";
  // If enough statistics compute all rates
  if (ShouldComputeRates(digitType::kPhysics))
    ComputeAllRates();

  ResetCounters(0, digitType::kPhysics);

  return true;
}

void MIDRatesComputerAlgorithm::ResetCounters(uint64_t newStartTS, digitType type)
{
  //  Reset all counters and timestamps
  std::for_each(fMapping.fStripVector.begin(), fMapping.fStripVector.end(),
                [](stripMapping& i) { i = stripMapping(); });

  fStructMaskSim.nNoisy = 0;
  fStructMaskSim.nDead = 0;
  fStructMaskSim.deadStripsIDs.clear();
  fStructMaskSim.noisyStripsIDs.clear();

  //    LOG(DEBUG) << "Reset counters in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

bool MIDRatesComputerAlgorithm::ShouldComputeRates(digitType type)
{
  // Return if enough statistics has been collected.
  // For the moment we simply check the number of received digits.
  // Customizable.
  return fCounter % 100 != 0;
}

void MIDRatesComputerAlgorithm::ComputeRate(stripMapping* strip)
{
  // Compute all the rates for a given strip
  for (int iType = 0; iType < digitType::kSize; ++iType) {
    strip->rate[iType] = (float_t)(strip->digitsCounter[iType]) / strip->area;

    uint64_t startTS = strip->startTS[iType];
    uint64_t stopTS = strip->stopTS[iType];

    if (stopTS > startTS) {
      strip->rate[iType] /= (stopTS - startTS);
    }
  }
}

void MIDRatesComputerAlgorithm::ComputeAllRates()
{
  // Compute all rates
  for (auto& stripIt : fMapping.fStripVector) {
    ComputeRate(&stripIt);
  }

  //    LOG(DEBUG) << "Rates computed in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

std::shared_ptr<std::vector<uint64_t>> MIDRatesComputerAlgorithm::Output()
{
  std::vector<uint64_t> digitsOut;
  digitsOut.resize(3 * fMapping.fStripVector.size());

  for (int iData = 0; iData < fMapping.fStripVector.size(); iData++) {
    for (int iType = 0; iType < digitType::kSize; iType++) {
      digitsOut[iData * 3 + iType] = fMapping.fStripVector[iData].digitsCounter[iType];
    }
  }

  return std::make_shared<std::vector<uint64_t>>(digitsOut);
}

} // namespace mid
} // namespace o2
