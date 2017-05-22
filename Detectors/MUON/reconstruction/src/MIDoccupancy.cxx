//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#include <TRandom.h>
#include <TMath.h>
#include "MUONReconstruction/MIDoccupancy.h"
#include "MUONBase/Deserializer.h"
#include "FairMQLogger.h"
#include "options/FairMQProgOptions.h"
#include "algorithm"

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
MIDoccupancy::~MIDoccupancy() {
    LOG(DEBUG) << "Detected noisy strips:";
    for(const auto &itMask : fStructMask.noisyStripsIDs){
        LOG(DEBUG) << "\t" << itMask << "\t\t" << fInternalMapping.at(itMask)->digitsCounter;
    }

    LOG(DEBUG) << "\nSimulated noisy strips:";
    for(const auto &itMaskSim : fStructMaskSim.noisyStripsIDs){
        if (fStructMask.noisyStripsIDs.find(itMaskSim) != fStructMask.noisyStripsIDs.end()) LOG(DEBUG) << "\t" << itMaskSim << "\t\t\tOK - found";
        else LOG(DEBUG) << "\t" << itMaskSim << "\t\t\tXX - not found";
    }
}

//_________________________________________________________________________________________________
void MIDoccupancy::InitTask() {

    LOG(INFO) << "Initializing device";

    std::string mapFilename = fConfig->GetValue<std::string>("binmapfile");

    if ( !(ReadMapping(mapFilename.c_str())) ){
        LOG(ERROR) << "Error reading the mapping from " << mapFilename;
    } else {
        LOG(INFO) << "Mapping correctly loaded.";
    }

    fStructsBuffer.reserve(64);

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
//        LOG(ERROR) << "Message empty, skipping";
        return true;
    }

    Deserializer MessageDeserializer(msg);
    int counter = 0;

    uint32_t *uniqueIDBuffer;

//    LOG(INFO) << "Received valid message";

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
        if ( gRandom->Rndm() > 0.99 ){
            strip->digitsCounter+=999999;
//            LOG(ERROR) << "Simulating noisy strip " << *uniqueIDBuffer;
            fStructMaskSim.noisyStripsIDs.insert(*uniqueIDBuffer).second;
            fStructMaskSim.nNoisy++;
        }

    }

    LOG(INFO) << "Received valid message containing "<<counter<<" digits";

    if ( !EnoughStatistics() ) {
        LOG(INFO) << "Not enough statistics: waiting for more.";
        return true;
    };

    MIDoccupancy::ComputeAllRates();

    const stripMapping* strip;
    uint32_t uniqueID;

    for(const auto &mapIterator : fInternalMapping){
        uniqueID = mapIterator.first;
        strip = mapIterator.second;

        if ( strip->isDead ) {

            auto alreadyThere = fStructMask.deadStripsIDs.insert(uniqueID).second;

            if(alreadyThere){
                fStructMask.nDead++;
//                LOG(ERROR)<<uniqueID<<" is dead.";
            } else {
//                LOG(INFO)<<uniqueID<<" already set.";
            }

        } else if ( strip->isNoisy ) {

            auto alreadyThere = fStructMask.noisyStripsIDs.insert(uniqueID).second;

            if(alreadyThere){
                fStructMask.nNoisy++;
//                LOG(ERROR)<<uniqueID<<" is noisy.";
            } else {
//                LOG(INFO)<<uniqueID<<" already set.";
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

//    fStripVector.reserve(23000);
    fInternalMapping.reserve(20992);

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

    Int_t nStrips = 0;
    Int_t nStrips2 = 0;

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
            //fInternalMapping.insert(std::pair<uint32_t, stripMapping>((uint32_t)padUniqueID, bufferStripMapping));
            fStripVector[nStrips2] = bufferStripMapping;
            fInternalMapping[padUniqueID] = &(fStripVector[nStrips2]);
            nStrips2++;

            //LOG(DEBUG) << "\t"<< padUniqueID <<" "<< bufferStripMapping.nNeighbours;
        }
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    LOG(DEBUG) << "Mapping loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";
    LOG(DEBUG) << nStrips << " " << nStrips2 << " loaded";

    return true;
}

//_________________________________________________________________________________________________
void MIDoccupancy::ResetCounters(uint64_t newStartTS) {

    auto tStart = std::chrono::high_resolution_clock::now();

    for( auto &vecIterator : fStripVector){
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
bool MIDoccupancy::EnoughStatistics() {
    long nOfActiveStrips = std::count_if(fStripVector.begin(),fStripVector.end(),[](stripMapping strip){ return strip.digitsCounter > 10; });
    return nOfActiveStrips > (0.001 * fStripVector.size());
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

    auto lambdaSortStrips = [](const stripMapping *a, const stripMapping *b) { return a->digitsCounter < b->digitsCounter; };
    auto lambdaIfNotZero = [](const stripMapping *strip){ return strip->digitsCounter>0; };
    auto lambdaSumDigits = [](uint64_t sum, const stripMapping *strip){ return sum + strip->digitsCounter; };


    stripMapping *strip;
    int previousColumnID = 0 ;

    for( auto &vecIteratorRead : fStripVector){
        strip = &vecIteratorRead;


        ComputeRate(strip);

//        LOG(DEBUG) << strip->digitsCounter;

        int currentColumnID = strip->columnID;

        if (previousColumnID != currentColumnID) {

            uint64_t nStrips = (uint64_t)std::count_if(fStructsBuffer.begin(),fStructsBuffer.end(),lambdaIfNotZero);

            if ( fStructsBuffer.size() == 0 ) {
                fStructsBuffer.clear();
                continue;
            }

            if ( nStrips<fStructsBuffer.size()/10 ){
                fStructsBuffer.clear();
                continue;
            }

//            LOG(DEBUG) << "Counting items (without zeroes) " << fStructsBuffer.size();

            std::sort(fStructsBuffer.begin(),fStructsBuffer.end(),lambdaSortStrips);
            uint64_t totalDigits = std::accumulate(fStructsBuffer.begin(),fStructsBuffer.end(),0ull,lambdaSumDigits);

            Double_t meanCounts = (Double_t)totalDigits/(Double_t)nStrips;
            Double_t nextMeanCounts = 0.;
            Double_t meanCountsSqrt = 0.;

            Int_t cutOut = 1;

//            LOG(DEBUG) << "Starting while loop";

            while ( cutOut < fStructsBuffer.size() ){

                totalDigits = std::accumulate(fStructsBuffer.begin(),fStructsBuffer.end()-cutOut,0ull,lambdaSumDigits);
                nStrips = (uint64_t)std::count_if(fStructsBuffer.begin(),fStructsBuffer.end()-cutOut,lambdaIfNotZero);

                if ( nStrips == 0 ) break;

                nextMeanCounts = (Double_t)totalDigits/(Double_t)nStrips;
                meanCountsSqrt = TMath::Sqrt(meanCounts);

                if ( meanCounts - nextMeanCounts < meanCountsSqrt ){
                    break;
                }

                meanCounts = nextMeanCounts;
                cutOut++;

            }

//            LOG(DEBUG) << "Mean counts for column " << currentColumnID << " are " << meanCounts << " obtained with " << cutOut << " calls.";

            for( const auto stripIterator : fStructsBuffer ){
                uint64_t digitsCounter = (*stripIterator).digitsCounter;
//                LOG(DEBUG) << digitsCounter;
                if ( digitsCounter > meanCounts * 42 /* So long and thanks for all the fish */){
//                    LOG(ERROR) << "Strip is noisy " << digitsCounter << " " << meanCounts;
                    (*stripIterator).isNoisy = true;
                }
//                else if ( digitsCounter < 0.01 * meanCounts ) (&*stripIterator)->isDead = true;
                else {
                    (*stripIterator).isNoisy = false;
                    (*stripIterator).isDead = false;
                }
            }

            previousColumnID = currentColumnID;

            fStructsBuffer.clear();
        }

        fStructsBuffer.emplace_back(strip);

//        LOG(DEBUG) << "Added Strip" << &vecIteratorRead;

    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    LOG(DEBUG) << "Rates computed in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

//_________________________________________________________________________________________________
bool MIDoccupancy::SendMask(){

    int msgMaskSize = sizeof(fStructMask.nDead) + sizeof(fStructMask.nNoisy) + (fStructMask.nDead + fStructMask.nNoisy) * sizeof(uint32_t);
    FairMQMessagePtr msgMask(NewMessage(msgMaskSize));

    uint32_t *msgAddr = reinterpret_cast<uint32_t*>(msgMask->GetData());

    UShort_t *nDead = reinterpret_cast<UShort_t*>(msgAddr);
    UShort_t *nNoisy = reinterpret_cast<UShort_t*>(&(nDead[1]));

    nDead = &(fStructMask.nDead);
    nNoisy = &(fStructMask.nNoisy);

    uint32_t *deadIDs = &(msgAddr[1]);
    uint32_t *noisyIDs = &(msgAddr[1+*nDead]);

    int iDead = 0;
    for( auto& deadIt : fStructMask.deadStripsIDs ){
        deadIDs[iDead++] = deadIt;
    }

    int iNoisy = 0;
    for( auto& noisyIt : fStructMask.noisyStripsIDs ){
        noisyIDs[iNoisy++] = noisyIt;
    }

    if ( Send(msgMask, "mask-out") < 0 ){
        LOG(ERROR) << "problem sending mask";
        return false;
    }

    return true;

}

