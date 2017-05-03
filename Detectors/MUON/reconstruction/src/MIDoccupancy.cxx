//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#include "MUONReconstruction/MIDoccupancy.h"
#include "MUONBase/Deserializer.h"
#include "FairMQLogger.h"
#include "options/FairMQProgOptions.h"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
MIDoccupancy::MIDoccupancy():
FairMQDevice(),
fInternalMapping(0x0),
fMapFilename(""){
    FairMQDevice::OnData("data-in", &MIDoccupancy::HandleData);
}

//_________________________________________________________________________________________________
MIDoccupancy::~MIDoccupancy() {}

//_________________________________________________________________________________________________
void MIDoccupancy::InitTask() {

    LOG(INFO) << "Initializing device";

    fMapFilename = fConfig->GetValue<std::string>("binmapfile");

    if ( !(ReadMapping(fMapFilename.c_str())) ){
        LOG(ERROR) << "Error reading the mapping from " << fMapFilename;
    } else {
        LOG(INFO) << "Mapping correctly loaded.";
    }

}

//_________________________________________________________________________________________________
bool MIDoccupancy::HandleData( FairMQMessagePtr &msg, int /*index*/ )
{

    if ( !msg ) {
        LOG(ERROR) << "Message pointer not valid, aborting";
        return false;
    }

    if ( msg->GetSize()<100 ) {
        LOG(ERROR) << "Message empty, skipping";
        return true;
    }

    Deserializer MessageDeserializer(msg);
    int counter = 0;

    Deserializer::deserializerDataStruct* deserializedData;

    while((deserializedData = MessageDeserializer.NextDigit())){
        counter++;
//        LOG(INFO) << "\t"<<deserializedData->fDetElemID;
//        LOG(INFO) << "\t"<<deserializedData->fBoardID;
//        LOG(INFO) << "\t"<<deserializedData->fChannel;
//        LOG(INFO) << "\t"<<deserializedData->fCathode;
    }

    LOG(INFO) << "Received valid message containing "<<counter<<" digits";
    return true;
}

//_________________________________________________________________________________________________
bool MIDoccupancy::ReadMapping( const char * filename )
{
    auto tStart = std::chrono::high_resolution_clock::now();

    LOG(INFO) << "Starting reading of mapping";

    int numberOfDetectionElements = 0;
    Mapping::mpDE* detectionElements = Mapping::ReadMapping(filename,numberOfDetectionElements);

    LOG(DEBUG) << "\t"<<numberOfDetectionElements<<" DE found";
    LOG(DEBUG) << "Initializing buffer struct";

    // struct to contain data from each strip
    stripMapping bufferStripMapping;

    // initialization of the buffer struct
    bufferStripMapping.startTS = 0;
    bufferStripMapping.stopTS = 0;
    bufferStripMapping.digitsCounter = 0;
    for (int iNeighboursInit = 0; iNeighboursInit < 10; ++iNeighboursInit) {
        bufferStripMapping.neighboursUniqueID[iNeighboursInit] = -1;
    }

    // loop over DE to read every pad (DE = detection element)
    for ( int iDE = 0; iDE < numberOfDetectionElements; iDE++ ){

        LOG(DEBUG) << "Processing DE "<<iDE;

        // read the iDE-th DE and the number of pads
        Mapping::mpDE& de(detectionElements[iDE]);
        int numberOfPads = de.nPads[0] + de.nPads[1] ;

        LOG(DEBUG) << "Starting map inversion";

        //  load the internal maps in order to get back to the UniqueID
        std::unordered_map<Long64_t, Long64_t> *padIndeces[2];
        padIndeces[0] = &(de.padIndices[0]);
        padIndeces[1] = &(de.padIndices[1]);

        // the two maps have to be reversed to make iPad->UniqueID
        std::unordered_map<Long64_t, Long64_t> reversedPadIndeces;
        for (auto indecesIt1 : *padIndeces[0]){
            reversedPadIndeces.insert({indecesIt1.second,indecesIt1.first});
        }
        for (auto indecesIt2 : *padIndeces[1]){
            reversedPadIndeces.insert({indecesIt2.second,indecesIt2.first});
        }

        // original maps not needed anymore
        padIndeces[0] = 0x0;
        padIndeces[1] = 0x0;

        // loop over pads from each DE
        for ( int iPad = 0; iPad < numberOfPads; iPad++ ){

            LOG(DEBUG) << "Processing pad "<<iPad;

            // read the iPad-th pad and the number of pads
            Mapping::mpPad& pad(de.pads[iPad]);
            int numberOfNeighbours = pad.nNeighbours;

            // load in the struct sensible data
            bufferStripMapping.nNeighbours = pad.nNeighbours;
            bufferStripMapping.area[0][0] = pad.area[0][0];
            bufferStripMapping.area[1][0] = pad.area[1][0];
            bufferStripMapping.area[0][1] = pad.area[0][1];
            bufferStripMapping.area[1][1] = pad.area[1][1];

            LOG(DEBUG) << "\t Pad has "<<numberOfNeighbours<<" neighbours";

            // load the neighboursUniqueIDs or set to -1 if no neighbour
            for ( int iNeighbours = 0; iNeighbours < numberOfNeighbours; iNeighbours++){
                Long64_t padUniqueID = reversedPadIndeces[pad.neighbours[iNeighbours]];

                LOG(DEBUG) <<"\t"<< iNeighbours <<" "<< padUniqueID;

                bufferStripMapping.neighboursUniqueID[iNeighbours] = padUniqueID;
            }
            for ( int iNeighbours = numberOfNeighbours; iNeighbours < 10; iNeighbours++){
                bufferStripMapping.neighboursUniqueID[iNeighbours] = -1;
            }

            LOG(DEBUG) << "Inserting the internal mapping entry";

            // save the buffer struct at the iPad position in the map
            fInternalMapping.insert(std::pair<Long64_t, stripMapping>(reversedPadIndeces[iPad], bufferStripMapping));

            LOG(DEBUG) << "\t"<< reversedPadIndeces[iPad] <<" "<< bufferStripMapping.nNeighbours;
        }
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    LOG(DEBUG) << "Mapping loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";

    return true;
}
