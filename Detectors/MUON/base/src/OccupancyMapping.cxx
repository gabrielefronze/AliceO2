//
// Created by Gabriele Gaetano Fronzé on 26/06/2017.
//

#include "MUONBase/OccupancyMapping.h"
#include <chrono>
#include "FairMQLogger.h"
#include "options/FairMQProgOptions.h"
#include "MUONBase/Mapping.h"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
bool OccupancyMapping::ReadMapping( const char * filename, int elementID ){

//    LOG(INFO) << "Starting reading of mapping for element "<<elementID;

    int numberOfDetectionElements = 0;
    Mapping::mpDE* detectionElements = Mapping::ReadMapping(filename,numberOfDetectionElements);
    
    // Check if the required element ID is
    if ( elementID < 0 || elementID > numberOfDetectionElements ) return false;
//
//    LOG(DEBUG) << "\t"<<numberOfDetectionElements<<" DE found";
//    LOG(DEBUG) << "Initializing buffer struct";

    // struct to contain data from each strip
    stripMapping bufferStripMapping;

    // initialization of the buffer struct
    bufferStripMapping.startTS[0] = 0;
    bufferStripMapping.startTS[1] = 0;
    bufferStripMapping.startTS[2] = 0;
    bufferStripMapping.stopTS[0] = 0;
    bufferStripMapping.stopTS[1] = 0;
    bufferStripMapping.stopTS[2] = 0;
    bufferStripMapping.rate[0] = 0;
    bufferStripMapping.rate[1] = 0;
    bufferStripMapping.rate[2] = 0;
    bufferStripMapping.digitsCounter[0] = 0;
    bufferStripMapping.digitsCounter[1] = 0;
    bufferStripMapping.digitsCounter[2] = 0;
    bufferStripMapping.isDead = false;
    bufferStripMapping.isNoisy = false;

    Int_t nStrips = 0;
    Int_t nStrips2 = 0;

    Mapping::mpDE& de(detectionElements[elementID]);
    int numberOfPads = de.nPads[0] + de.nPads[1] ;

//        LOG(DEBUG) << "Starting map inversion";

    //  load the internal maps in order to get back to the UniqueID
    std::unordered_map<Long64_t, Long64_t> *padIndeces[2];
    padIndeces[0] = &(de.padIndices[0]);
    padIndeces[1] = &(de.padIndices[1]);

    // the two maps have to be reversed to make iPad->UniqueID
    std::unordered_map<uint32_t, uint32_t> reversedPadIndexes;

    // Using indecesIt1.second-1 because of theway the mapping has been filled
    for (const auto &indecesIt1 : *padIndeces[0]){
        reversedPadIndexes.insert({(uint32_t)indecesIt1.second-1,(uint32_t)indecesIt1.first});
    }
    for (const auto &indecesIt2 : *padIndeces[1]){
        reversedPadIndexes.insert({(uint32_t)indecesIt2.second-1,(uint32_t)indecesIt2.first});
    }

    // original maps not needed anymore
    padIndeces[0] = 0x0;
    padIndeces[1] = 0x0;

    nStrips += numberOfPads;

    // reserving the needed space for the following method
    fStripVector.reserve((unsigned int)numberOfPads);
    fIDMap.reserve((unsigned int)numberOfPads);

    // loop over pads from each DE
    for ( uint32_t iPad = 0; iPad < numberOfPads; iPad++ ){

//            LOG(DEBUG) << "Processing pad "<<iPad;

        // read the iPad-th pad and the number of pads
        Mapping::mpPad& pad(de.pads[iPad]);

        bufferStripMapping.coord[0][0] = pad.area[0][0];
        bufferStripMapping.coord[0][1] = pad.area[0][1];
        bufferStripMapping.coord[1][0] = pad.area[1][0];
        bufferStripMapping.coord[1][1] = pad.area[1][1];

        Float_t deltaX = pad.area[0][1] - pad.area[0][0];
        Float_t deltaY = - pad.area[1][0] + pad.area[1][1];
        bufferStripMapping.area = deltaX * deltaY;
        bufferStripMapping.columnID = pad.iDigit;

//            LOG(DEBUG) << "Inserting the internal mapping entry";

        uint32_t padUniqueID;
        try {
            padUniqueID = reversedPadIndexes.at(iPad);
        } catch ( std::out_of_range err ){
            LOG(ERROR) << "No reverse mapping found for pad "<< iPad;
            LOG(ERROR) << "Aborting...";
            return false;
        }

        // save the buffer struct at the iPad position in the map
        //fIDMap.insert(std::pair<uint32_t, stripMapping>((uint32_t)padUniqueID, bufferStripMapping));
        fStripVector.emplace_back(bufferStripMapping);
        fIDMap[padUniqueID] = &(fStripVector.back());
        nStrips2++;

        //LOG(DEBUG) << "\t"<< padUniqueID <<" "<< bufferStripMapping.nNeighbours;
    }

    return true;
}

//_________________________________________________________________________________________________
bool OccupancyMapping::ReadMapping( const char * filename,std::vector<int> elementIDs){
    auto tStart = std::chrono::high_resolution_clock::now();

    int counter = 0;
    for(const auto &itElementID : elementIDs){
        if (OccupancyMapping::ReadMapping(filename,itElementID)) counter++;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    LOG(DEBUG) << "Mapping for "<<counter<<" elements loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";
    LOG(DEBUG) << counter << " " << elementIDs.size() << " loaded";

    return counter==elementIDs.size();
}

//_________________________________________________________________________________________________
bool OccupancyMapping::ReadMapping( const char * filename )
{
    auto tStart = std::chrono::high_resolution_clock::now();

    int numberOfDetectionElements = 0;
    Mapping::mpDE* detectionElements = Mapping::ReadMapping(filename,numberOfDetectionElements);

    int counter = 0;
    for ( int iDE = 0; iDE < numberOfDetectionElements; iDE++ ){
        if (OccupancyMapping::ReadMapping(filename,iDE)) counter++;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    LOG(DEBUG) << "Mapping for "<<counter<<" elements loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";
    LOG(DEBUG) << counter << " " << numberOfDetectionElements << " loaded";

    return counter==numberOfDetectionElements;
}