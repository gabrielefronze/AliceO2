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

    uint32_t *uniqueIDBuffer;
//    Deserializer::deserializerDataStruct* deserializedData;

//    while((deserializedData = MessageDeserializer.NextDigit())){
//        counter++;
////        LOG(INFO) << "\t"<<deserializedData->fDetElemID;
////        LOG(INFO) << "\t"<<deserializedData->fBoardID;
////        LOG(INFO) << "\t"<<deserializedData->fChannel;
////        LOG(INFO) << "\t"<<deserializedData->fCathode;
//    }

    LOG(INFO) << "Received valid message";

    while((uniqueIDBuffer = MessageDeserializer.NextUniqueID())){
        LOG(INFO) << "UniqueID "<<  *uniqueIDBuffer;

        if ( ((uniqueIDBuffer & 0xFFF) / 100) <10 ) continue;

        stripMapping* strip;

        try {
            strip = &fInternalMapping.at((uint64_t)(*uniqueIDBuffer));
        } catch (std::out_of_range err){
            LOG(ERROR) << "No stripMapping struct found for ID "<< *uniqueIDBuffer;
            LOG(ERROR) << "Continuing...";
            continue;
        }

        strip->digitsCounter++;

    }

    LOG(INFO) << "Received valid message containing "<<counter<<" digits";
    MIDoccupancy::ComputeAllIsDead();
    MIDoccupancy::ComputeAllIsNoisy();

    for(auto mapIterator : fInternalMapping){
        if ( mapIterator.second.isDead ) LOG(INFO)<<mapIterator.first<<" is dead.";
        else if ( mapIterator.second.isNoisy ) LOG(INFO)<<mapIterator.first<<" is noisy.";
    }

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
    bufferStripMapping.rate = 0;
    bufferStripMapping.isDead = false;
    bufferStripMapping.isNoisy = false;
    bufferStripMapping.useMe = true;

    for (int iNeighboursInit = 0; iNeighboursInit < 10; ++iNeighboursInit) {
        bufferStripMapping.neighboursUniqueIDs[iNeighboursInit] = 7777777;
    }

    // loop over DE to read every pad (DE = detection element)
    for ( int iDE = 0; iDE < numberOfDetectionElements; iDE++ ){

//        LOG(DEBUG) << "Processing DE "<<iDE;

        // read the iDE-th DE and the number of pads
        Mapping::mpDE& de(detectionElements[iDE]);
        int numberOfPads = de.nPads[0] + de.nPads[1] ;

//        LOG(DEBUG) << "Starting map inversion";

        //  load the internal maps in order to get back to the UniqueID
        std::unordered_map<Long64_t, Long64_t> *padIndeces[2];
        padIndeces[0] = &(de.padIndices[0]);
        padIndeces[1] = &(de.padIndices[1]);

        // the two maps have to be reversed to make iPad->UniqueID
        std::unordered_map<uint64_t, uint64_t> reversedPadIndexes;

        // Using indecesIt1.second-1 because of theway the mapping has been filled
        for (auto indecesIt1 : *padIndeces[0]){
            reversedPadIndexes.insert({indecesIt1.second-1,indecesIt1.first});
        }
        for (auto indecesIt2 : *padIndeces[1]){
            reversedPadIndexes.insert({indecesIt2.second-1,indecesIt2.first});
        }

        // original maps not needed anymore
        padIndeces[0] = 0x0;
        padIndeces[1] = 0x0;

        // loop over pads from each DE
        for ( uint64_t iPad = 0; iPad < numberOfPads; iPad++ ){

//            LOG(DEBUG) << "Processing pad "<<iPad;

            // read the iPad-th pad and the number of pads
            Mapping::mpPad& pad(de.pads[iPad]);
            int numberOfNeighbours = pad.nNeighbours;

            // load in the struct sensible data
            bufferStripMapping.nNeighbours = pad.nNeighbours;

            Float_t deltaX = pad.area[0][1] - pad.area[0][0];
            Float_t deltaY = pad.area[1][0] - pad.area[1][1];
            bufferStripMapping.area = deltaX * deltaY;

//            LOG(DEBUG) << "\t Pad has "<<numberOfNeighbours<<" neighbours";

            //TODO: make the parsing avoid adding lateral neighbours
            // load the neighboursUniqueIDs or set to -1 if no neighbour
            for ( int iNeighbours = 0; iNeighbours < numberOfNeighbours; iNeighbours++){
                uint64_t neighbourUniqueID;
                try {
                    neighbourUniqueID = reversedPadIndexes.at(pad.neighbours[iNeighbours]);
                } catch ( std::out_of_range err ){
                    LOG(ERROR) << "No reverse mapping found for pad "<< pad.neighbours[iNeighbours] <<" neighbour of "<< iPad;
                    LOG(ERROR) << "Aborting...";
                    return false;
                }
                
//                LOG(DEBUG) <<"\t"<< iNeighbours <<" "<< neighbourUniqueID;

                bufferStripMapping.neighboursUniqueIDs[iNeighbours] = neighbourUniqueID;
            }
            for ( int iNeighbours = numberOfNeighbours; iNeighbours < 10; iNeighbours++){
                bufferStripMapping.neighboursUniqueIDs[iNeighbours] = 7777777;
            }

//            LOG(DEBUG) << "Inserting the internal mapping entry";

            Long64_t padUniqueID;
            try {
                padUniqueID = reversedPadIndexes.at(iPad);
            } catch ( std::out_of_range err ){
                LOG(ERROR) << "No reverse mapping found for pad "<< iPad;
                LOG(ERROR) << "Aborting...";
                return false;
            }

            // save the buffer struct at the iPad position in the map
            fInternalMapping.insert(std::pair<Long64_t, stripMapping>(padUniqueID, bufferStripMapping));

            LOG(DEBUG) << "\t"<< padUniqueID <<" "<< bufferStripMapping.nNeighbours;
        }
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    LOG(DEBUG) << "Mapping loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";

    return true;
}

//_________________________________________________________________________________________________
void MIDoccupancy::ResetUseMe(Bool_t value) {

    auto tStart = std::chrono::high_resolution_clock::now();

    for(auto mapIterator : fInternalMapping){
        mapIterator.second.useMe = value;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    LOG(DEBUG) << "Reset counters in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

//_________________________________________________________________________________________________
void MIDoccupancy::ResetCounters(uint64_t newStartTS) {

    auto tStart = std::chrono::high_resolution_clock::now();

    for(auto mapIterator : fInternalMapping){
        stripMapping* strip = &(mapIterator.second);
        strip->digitsCounter=0;
        strip->startTS=newStartTS;
        strip->stopTS=0;
        strip->isNoisy = false;
        strip->isDead = false;
        strip->useMe = true;
        strip->rate = 0;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    LOG(DEBUG) << "Reset counters in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

//_________________________________________________________________________________________________
void MIDoccupancy::ComputeRate(stripMapping* strip) {
    strip->rate = (Float_t)(strip->digitsCounter) / strip->area;

    uint64_t startTS = strip->startTS;
    uint64_t stopTS = strip->stopTS;

    if ( stopTS > startTS ){
        strip->rate/=(stopTS-startTS);
    }
}

//_________________________________________________________________________________________________
void MIDoccupancy::ComputeAllRates() {

    auto tStart = std::chrono::high_resolution_clock::now();

    for(auto mapIterator : fInternalMapping){
        ComputeRate(&(mapIterator.second));
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    LOG(DEBUG) << "Rates computed in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

//_________________________________________________________________________________________________
double MIDoccupancy::GetMeanRate(stripMapping* strip, uint depth){

    MIDoccupancy::ResetUseMe(true);

    uint counter = 0;

    double rateSum = RecursiveGetRateSum(strip, counter, depth);

    MIDoccupancy::ResetUseMe(true);

    return rateSum/(double)counter;

};

//_________________________________________________________________________________________________
double MIDoccupancy::RecursiveGetRateSum(stripMapping* strip, uint &counter, uint depth){
    double rateSum = 0.;
    counter = 0;
    if ( depth>=1 ){
        Int_t nNeighbours = strip->nNeighbours;
        for (int iNeighbours = 0; iNeighbours < nNeighbours; ++iNeighbours) {

            MIDoccupancy::stripMapping* neighbourStrip;
            try {
                neighbourStrip = &(fInternalMapping.at(strip->neighboursUniqueIDs[iNeighbours]));
            } catch ( std::out_of_range err ){
                LOG(ERROR) << "Missing entry in the mapping. Continuing.";
                continue;
            }

            if (neighbourStrip->useMe) rateSum += neighbourStrip->rate;
            neighbourStrip->useMe = false;
            counter++;
            rateSum += RecursiveGetRateSum(neighbourStrip, counter, depth-1);
        }
    }
    return rateSum;
}

//_________________________________________________________________________________________________
void MIDoccupancy::ComputeIsDead(stripMapping* strip) {

    double meanRate = GetMeanRate(strip);

    if ( strip->rate > meanRate*0.000001 ) strip->isDead = true;
}

//_________________________________________________________________________________________________
void MIDoccupancy::ComputeAllIsDead() {

    auto tStart = std::chrono::high_resolution_clock::now();

    for(auto mapIterator : fInternalMapping){
        ComputeIsDead(&(mapIterator.second));
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    LOG(DEBUG) << "Dead strips computed in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

//_________________________________________________________________________________________________
void MIDoccupancy::ComputeIsNoisy(stripMapping* strip) {

    double meanRate = GetMeanRate(strip);

    if ( strip->rate > meanRate*100. ) strip->isNoisy = true;
}

//_________________________________________________________________________________________________
void MIDoccupancy::ComputeAllIsNoisy() {

    auto tStart = std::chrono::high_resolution_clock::now();

    for(auto mapIterator : fInternalMapping){
        ComputeIsNoisy(&(mapIterator.second));
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    LOG(DEBUG) << "Dead strips computed in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}
