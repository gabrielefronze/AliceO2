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
fInternalMapping(0x0)
{

    fStructMask.nDead = 0;
    fStructMask.nNoisy = 0;

    FairMQDevice::OnData("data-in", &MIDoccupancy::HandleData);
}

//_________________________________________________________________________________________________
MIDoccupancy::~MIDoccupancy() {}

//_________________________________________________________________________________________________
void MIDoccupancy::InitTask() {

    LOG(INFO) << "Initializing device";

    std::string mapFilename = fConfig->GetValue<std::string>("binmapfile");

    if ( !(ReadMapping(mapFilename.c_str())) ){
        LOG(ERROR) << "Error reading the mapping from " << mapFilename;
    } else {
        LOG(INFO) << "Mapping correctly loaded.";
    }

    std::cout<< &fInternalMapping << std::endl;

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

    LOG(INFO) << "Received valid message";

    while((uniqueIDBuffer = MessageDeserializer.NextUniqueID())){
//        LOG(INFO) << "UniqueID "<<  ((*uniqueIDBuffer) & 0xFFF);

        if ( ((*uniqueIDBuffer) & 0xFFF) < 1100 ) continue;

        counter++;

        stripMapping* strip;

        try {
            strip = fInternalMapping.at(*uniqueIDBuffer);
        } catch (std::out_of_range err){
            LOG(ERROR) << "No stripMapping struct found for ID "<< *uniqueIDBuffer;
            continue;
        }

        strip->digitsCounter++;

    }

    LOG(INFO) << "Received valid message containing "<<counter<<" digits";
    MIDoccupancy::ComputeAllRates();

    const stripMapping* strip;
    uint32_t uniqueID;

    for(auto mapIterator : fInternalMapping){
        uniqueID = mapIterator.first;
        strip = mapIterator.second;

        if ( strip->isDead ) {

            auto alreadyThere = fStructMask.deadStripsIDs.insert(uniqueID).second;

            if(alreadyThere){
                fStructMask.nDead++;
                LOG(INFO)<<uniqueID<<" is dead.";
            } else {
                LOG(INFO)<<uniqueID<<" already set.";
            }

        } else if ( strip->isNoisy ) {

            auto alreadyThere = fStructMask.noisyStripsIDs.insert(uniqueID).second;

            if(alreadyThere){
                fStructMask.nNoisy++;
                LOG(INFO)<<uniqueID<<" is noisy.";
            } else {
                LOG(INFO)<<uniqueID<<" already set.";
            }

        }
//        else LOG(INFO)<<uniqueID<<" is working as expected.";
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

    fStripVector.reserve(23000);
    fInternalMapping.reserve(23000);

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
        std::unordered_map<uint32_t, uint32_t> reversedPadIndexes;

        // Using indecesIt1.second-1 because of theway the mapping has been filled
        for (auto indecesIt1 : *padIndeces[0]){
            reversedPadIndexes.insert({(uint32_t)indecesIt1.second-1,(uint32_t)indecesIt1.first});
        }
        for (auto indecesIt2 : *padIndeces[1]){
            reversedPadIndexes.insert({(uint32_t)indecesIt2.second-1,(uint32_t)indecesIt2.first});
        }

        // original maps not needed anymore
        padIndeces[0] = 0x0;
        padIndeces[1] = 0x0;

        // loop over pads from each DE
        for ( uint32_t iPad = 0; iPad < numberOfPads; iPad++ ){

//            LOG(DEBUG) << "Processing pad "<<iPad;

            // read the iPad-th pad and the number of pads
            Mapping::mpPad& pad(de.pads[iPad]);

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
            //fInternalMapping.insert(std::pair<uint32_t, stripMapping>((uint32_t)padUniqueID, bufferStripMapping));
            fStripVector.emplace_back(bufferStripMapping);
            fInternalMapping[padUniqueID] = &(fStripVector.back());

            //LOG(DEBUG) << "\t"<< padUniqueID <<" "<< bufferStripMapping.nNeighbours;
        }
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    LOG(DEBUG) << "Mapping loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";

    return true;
}

//_________________________________________________________________________________________________
void MIDoccupancy::ResetCounters(uint64_t newStartTS) {

    auto tStart = std::chrono::high_resolution_clock::now();

    for(auto vecIterator : fStripVector){
        stripMapping* strip = &vecIterator;
        strip->digitsCounter=0;
        strip->startTS=newStartTS;
        strip->stopTS=0;
        strip->isNoisy = false;
        strip->isDead = false;
        strip->rate = 0;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

//    LOG(DEBUG) << "Reset counters in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
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

    stripMapping *strip;
    int previousColumnID = 0 ;
    Float_t ratesSum = 0;
    Int_t nRates = 0;


    for(auto vecIteratorRead : fStripVector){
        strip = &vecIteratorRead;
        ComputeRate(strip);
        int currentColumnID = strip->columnID;

        if (previousColumnID != currentColumnID) {

            if ( nRates == 0 ) continue;

            Float_t meanRate = ratesSum / (Float_t)nRates;

            for (int iStrip = 0; iStrip < nRates; ++iStrip) {
                Float_t rate = (*fStructsBuffer[iStrip]).rate;

                if ( rate < 0.001 * meanRate ) (*fStructsBuffer[iStrip]).isDead = true;
                else if ( rate > 100000 * meanRate ) (*fStructsBuffer[iStrip]).isNoisy = true;
            }

            previousColumnID = currentColumnID;
            ratesSum = strip->rate;
            nRates = 1;
        } else {
            ratesSum += strip->rate;
            nRates++;
        }

        fStructsBuffer[nRates-1] = &vecIteratorRead;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    LOG(DEBUG) << "Rates computed in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}
