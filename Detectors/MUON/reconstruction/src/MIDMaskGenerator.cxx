//
// Created by Gabriele Gaetano Fronzé on 23/06/2017.
//

#include "MUONReconstruction/MIDMaskGenerator.h"
#include "options/FairMQProgOptions.h"
#include "TMath.h"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
MIDMaskGenerator::MIDMaskGenerator():
        FairMQDevice()
{
    fStructMask.nDead = 0;
    fStructMask.nNoisy = 0;

    OnData("rates-in", &MIDMaskGenerator::HandleData);
}

//_________________________________________________________________________________________________
void MIDMaskGenerator::InitTask() {

    LOG(INFO) << "Initializing device";

    // Loading mapping at startup
    std::string mapFilename = fConfig->GetValue<std::string>("binmapfile");
    if ( !(ReadMapping(mapFilename.c_str())) ){
        LOG(ERROR) << "Error reading the mapping from " << mapFilename;
    } else {
        LOG(INFO) << "Mapping correctly loaded.";
    }

    return;
}

//_________________________________________________________________________________________________
bool MIDMaskGenerator::HandleData( FairMQMessagePtr &msg, int /*index*/ ){

    uint64_t msgSize = fStripVector.size() * digitType::kSize;

    Float_t *dataPointer = reinterpret_cast<Float_t*>(msg->GetData());

    // Copy the payload of the message in the internal data container
    for ( int iData = 0; iData < fStripVector.size(); iData++ ) {
        for (int iType = 0; iType < digitType::kSize; iType++ ) {
            fStripVector[iData].rate[iType] = dataPointer[iData*3 + iType];
        }
    }

    MIDMaskGenerator::FindNoisy(digitType::kPhysics);
    MIDMaskGenerator::FindDead(digitType::kPhysics);

    return MIDMaskGenerator::SendMask();
}

//_________________________________________________________________________________________________
void MIDMaskGenerator::FindNoisy(digitType type){

    auto lambdaSortStrips = [type](const stripMapping *a, const stripMapping *b)->bool{ return a->digitsCounter[type] < b->digitsCounter[type]; };
    auto lambdaIfNotZero = [type](const stripMapping *strip)->bool{ return strip->digitsCounter[type]>0; };
    auto lambdaSumDigits = [type](uint64_t sum, const stripMapping *strip)->uint64_t{ return sum + strip->digitsCounter[type]; };

    auto tStart = std::chrono::high_resolution_clock::now();

    stripMapping *strip;
    int previousColumnID = 0 ;

    for( auto &vecIteratorRead : fStripVector){
        strip = &vecIteratorRead;

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

            //TODO: indeed might be better to use a fixed size for loop!
            while ( (nStrips = (uint64_t)std::count_if(fStructsBuffer.begin(),fStructsBuffer.end()-cutOut,lambdaIfNotZero)) > 0 ){

                totalDigits = std::accumulate(fStructsBuffer.begin(),fStructsBuffer.end()-cutOut,0ull,lambdaSumDigits);

                nextMeanCounts = (Double_t)totalDigits/(Double_t)nStrips;

                if ( meanCounts - nextMeanCounts < meanCountsSqrt ){
                    break;
                }

                meanCountsSqrt = TMath::Sqrt(meanCounts);

                meanCounts = nextMeanCounts;
                cutOut++;
            }

//            LOG(DEBUG) << "Mean counts for column " << currentColumnID << " are " << meanCounts << " obtained with " << cutOut << " calls.";

            for( const auto &stripIterator : fStructsBuffer ){
//                LOG(DEBUG) << digitsCounter;
                (*stripIterator).isNoisy = ( (*stripIterator).digitsCounter[type] > meanCounts * 42 /* So long and thanks for all the fish */);
            }

            previousColumnID = currentColumnID;

            fStructsBuffer.clear();
        }

        fStructsBuffer.emplace_back(strip);

//        LOG(DEBUG) << "Added Strip" << &vecIteratorRead;

    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    LOG(DEBUG) << "Rates computed in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
    return;
}

//_________________________________________________________________________________________________
void MIDMaskGenerator::FindDead(digitType type){
    return;
}

//_________________________________________________________________________________________________
void MIDMaskGenerator::ResetAll(){

    for( auto &vecIteratorRead : fStripVector){
        vecIteratorRead.isNoisy = kFALSE;
        vecIteratorRead.isDead = kFALSE;
    }

    fStructMask.nNoisy = 0;
    fStructMask.nDead = 0;
    fStructMask.deadStripsIDs.clear();
    fStructMask.noisyStripsIDs.clear();

    return;
};

//_________________________________________________________________________________________________
void MIDMaskGenerator::FillMask(){

    for(const auto &mapIterator : fInternalMapping){
        auto uniqueID = mapIterator.first;
        auto strip = mapIterator.second;

        if ( strip->isDead ) {

            auto alreadyThere = fStructMask.deadStripsIDs.insert(uniqueID).second;

            if(alreadyThere){
                LOG(ERROR)<<uniqueID<<" is dead.";
            } else {
                LOG(INFO)<<uniqueID<<" already set.";
            }

        } else if ( strip->isNoisy ) {

            auto alreadyThere = fStructMask.noisyStripsIDs.insert(uniqueID).second;

            if(alreadyThere){
                LOG(ERROR)<<uniqueID<<" is noisy.";
            } else {
                LOG(INFO)<<uniqueID<<" already set.";
            }

        }
//        else LOG(INFO)<<uniqueID<<" is working as expected.";
    }

    fStructMask.nDead = (UShort_t)fStructMask.deadStripsIDs.size();
    fStructMask.nNoisy = (UShort_t)fStructMask.noisyStripsIDs.size();

    return;
};

//_________________________________________________________________________________________________
bool MIDMaskGenerator::SendMask(){
    return true;
}