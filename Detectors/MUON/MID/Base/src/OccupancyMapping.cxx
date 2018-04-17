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

#include "MIDBase/OccupancyMapping.h"
#include <chrono>
#include "FairMQLogger.h"
#include "MIDBase/FilteringMapping.h"
#include "options/FairMQProgOptions.h"

using namespace o2::mid;

//_________________________________________________________________________________________________
bool OccupancyMapping::ReadMapping(const char* filename, int elementID)
{
  //    LOG(INFO) << "Starting reading of mapping for element "<<elementID;

  int numberOfDetectionElements = 0;
  FilteringMapping::mpDE* detectionElements = FilteringMapping::ReadMapping(filename, numberOfDetectionElements);

  // Check if the required element ID is
  if (elementID < 0 || elementID > numberOfDetectionElements)
    return false;

  LOG(DEBUG) << "\t" << elementID << " DE found ";

  Int_t nStrips = 0;
  Int_t nStrips2 = 0;

  FilteringMapping::mpDE& de(detectionElements[elementID]);
  int numberOfPads = de.nPads[0] + de.nPads[1];

  //        LOG(DEBUG) << "Starting map inversion";

  //  load the internal maps in order to get back to the UniqueID
  std::unordered_map<Long64_t, Long64_t>* padIndeces[2];
  padIndeces[0] = &(de.padIndices[0]);
  padIndeces[1] = &(de.padIndices[1]);

  // the two maps have to be reversed to make iPad->UniqueID
  std::unordered_map<uint32_t, uint32_t> reversedPadIndexes;

  // Using indecesIt1.second-1 because of the way the mapping has been filled
  for (const auto& indecesIt1 : *padIndeces[0]) {
    reversedPadIndexes.insert(std::make_pair((uint32_t)indecesIt1.second - 1, (uint32_t)indecesIt1.first));
  }
  for (const auto& indecesIt2 : *padIndeces[1]) {
    reversedPadIndexes.insert(std::make_pair((uint32_t)indecesIt2.second - 1, (uint32_t)indecesIt2.first));
  }

  // original maps not needed anymore
  padIndeces[0] = nullptr;
  padIndeces[1] = nullptr;

  nStrips += numberOfPads;

  // reserving the needed space for the following method
  fStripVector.reserve((unsigned int)numberOfPads);
  //    fIDMap.reserve((unsigned int)numberOfPads);

  auto vectSize = fStripVector.size();

  // loop over pads from each DE
  for (uint32_t iPad = 0; iPad < numberOfPads; iPad++) {
    // struct to contain data from each strip
    //    LOG(DEBUG) << "Initializing buffer struct";
    stripMapping bufferStripMapping = stripMapping();

    //            LOG(DEBUG) << "Processing pad "<<iPad;

    // read the iPad-th pad and the number of pads
    FilteringMapping::mpPad& pad(de.pads[iPad]);

    bufferStripMapping.coord[0][0] = (float_t)pad.area[0][0];
    bufferStripMapping.coord[0][1] = (float_t)pad.area[0][1];
    bufferStripMapping.coord[1][0] = (float_t)pad.area[1][0];
    bufferStripMapping.coord[1][1] = (float_t)pad.area[1][1];

    float_t deltaX = (float_t)pad.area[0][1] - (float_t)pad.area[0][0];
    float_t deltaY = -(float_t)pad.area[1][0] + (float_t)pad.area[1][1];
    bufferStripMapping.area = deltaX * deltaY;
    bufferStripMapping.columnID = (ushort_t)pad.iDigit;

    //            LOG(DEBUG) << "Inserting the internal mapping entry";

    // save the buffer struct at the iPad position in the map
    // fIDMap.insert(std::pair<uint32_t, stripMapping>((uint32_t)padUniqueID, bufferStripMapping));
    fStripVector.emplace_back(bufferStripMapping);
    //        fIDMap[padUniqueID] = &(fStripVector.back());

    uint32_t padUniqueID;
    auto IDFinder = reversedPadIndexes.find(iPad);
    if (IDFinder == reversedPadIndexes.end()) {
      LOG(ERROR) << "No ID found for pad " << iPad;
      continue;
    }
    padUniqueID = IDFinder->second;

    //        LOG(DEBUG) << (padUniqueID & 0xFFF) << " " << iPad;
    fIDMap.insert(std::make_pair(padUniqueID, fStripVector.size()));

    nStrips2++;

    // LOG(DEBUG) << "\t"<< padUniqueID <<" "<< bufferStripMapping.nNeighbours;
  }

  return nStrips == nStrips2;
}

//_________________________________________________________________________________________________
bool OccupancyMapping::ReadMapping(const char* filename, std::vector<int> elementIDs)
{
  auto tStart = std::chrono::high_resolution_clock::now();

  int counter = 0;
  for (const auto& itElementID : elementIDs) {
    if (OccupancyMapping::ReadMapping(filename, itElementID))
      counter++;
  }

  auto tEnd = std::chrono::high_resolution_clock::now();
  LOG(DEBUG) << "Mapping for " << counter
             << " elements loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";
  LOG(DEBUG) << counter << " " << elementIDs.size() << " loaded";

  return counter == elementIDs.size();
}

//_________________________________________________________________________________________________
bool OccupancyMapping::ReadMapping(const char* filename)
{
  auto tStart = std::chrono::high_resolution_clock::now();

  int numberOfDetectionElements = 0;
  FilteringMapping::mpDE* detectionElements = FilteringMapping::ReadMapping(filename, numberOfDetectionElements);

  int counter = 0;
  for (int iDE = 0; iDE < numberOfDetectionElements; iDE++) {
    if (OccupancyMapping::ReadMapping(filename, iDE))
      counter++;
  }

  auto tEnd = std::chrono::high_resolution_clock::now();
  LOG(DEBUG) << "Mapping for " << counter
             << " elements loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";
  LOG(DEBUG) << counter << " " << numberOfDetectionElements << " loaded";

  return counter == numberOfDetectionElements;
}

stripMapping::stripMapping()
{
  for (int iDigitType = 0; iDigitType < digitType::kSize; iDigitType++) {
    startTS[iDigitType] = 0;
    stopTS[iDigitType] = 0;
    rate[iDigitType] = 0;
    digitsCounter[iDigitType] = 0;
  }
  columnID = 0;
  area = 0;
  coord[0][0] = 0;
  coord[0][1] = 0;
  coord[1][1] = 0;
  coord[1][0] = 0;
  isDead = false;
  isNoisy = false;
}
