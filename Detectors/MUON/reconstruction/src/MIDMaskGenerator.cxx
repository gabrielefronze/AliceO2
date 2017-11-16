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
MIDMaskGenerator::~MIDMaskGenerator(){
    LOG(DEBUG) << "Detected noisy strips:";
    for(const auto &itMask : fStructMask.noisyStripsIDs){
        LOG(DEBUG) << "\t" << itMask << "\t\t" << fMapping[itMask]->digitsCounter[digitType::kPhysics];
    }
}

//_________________________________________________________________________________________________
void MIDMaskGenerator::InitTask() {

    LOG(INFO) << "Initializing device";

    // Loading mapping at startup
    std::string mapFilename = fConfig->GetValue<std::string>("binmapfile");
    if ( !(fMapping.ReadMapping(mapFilename.c_str())) ){
        LOG(ERROR) << "Error reading the mapping from " << mapFilename;
    } else {
        LOG(INFO) << "Mapping correctly loaded.";
    }
}

//_________________________________________________________________________________________________
bool MIDMaskGenerator::HandleData( FairMQMessagePtr &msg, int /*index*/ ){

    // If the message is empty something is going wrong. The process should be aborted.
    if ( !msg ) {
        LOG(ERROR) << "Message pointer not valid, aborting";
        return false;
    }

    // If the input is smaller than the header size the message is empty and we should skip.
    if ( msg->GetSize() < 1 ) {
        LOG(ERROR) << "Message empty, skipping";
        return true;
    }

    LOG(DEBUG) << "Received valid message";

    uint64_t *dataPointer = reinterpret_cast<uint64_t*>(msg->GetData());

    // Copy the payload of the message in the internal data container
    for ( int iData = 0; iData < fMapping.fStripVector.size(); iData++ ) {
        for (int iType = 0; iType < digitType::kSize; iType++ ) {
            fMapping.fStripVector[iData].digitsCounter[iType] = dataPointer[iData*3 + iType];
        }
    }

    LOG(DEBUG) << "Message parsing done!";

    MIDMaskGenerator::FindNoisy(digitType::kPhysics);
    MIDMaskGenerator::FindDead(digitType::kPhysics);

    MIDMaskGenerator::FillMask();

    LOG(DEBUG) << "Sending mask...";

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

    for( auto &vecIteratorRead : fMapping.fStripVector){
        strip = &vecIteratorRead;


//        ComputeRate(strip);

//        LOG(DEBUG) << strip->digitsCounter;

        int currentColumnID = strip->columnID;

        if (previousColumnID != currentColumnID) {

            uint64_t nStrips = (uint64_t)std::count_if(fMapping.fStructsBuffer.begin(),fMapping.fStructsBuffer.end(),lambdaIfNotZero);

            if ( nStrips == 0 ) continue;

            if ( fMapping.fStructsBuffer.size() == 0 ) {
                fMapping.fStructsBuffer.clear();
                continue;
            }

            if ( nStrips<fMapping.fStructsBuffer.size()/10 ){
                fMapping.fStructsBuffer.clear();
                continue;
            }

//            LOG(DEBUG) << "Counting items (without zeroes) " << fStructsBuffer.size();

            std::sort(fMapping.fStructsBuffer.begin(),fMapping.fStructsBuffer.end(),lambdaSortStrips);
            uint64_t totalDigits = std::accumulate(fMapping.fStructsBuffer.begin(),fMapping.fStructsBuffer.end(),0ull,lambdaSumDigits);

            Double_t meanCounts = (Double_t)totalDigits/(Double_t)nStrips;
            Double_t nextMeanCounts = 0.;
            Double_t meanCountsSqrt = 0.;

            Int_t cutOut = 1;

//            LOG(DEBUG) << "Starting while loop";

            while ( cutOut < fMapping.fStructsBuffer.size() ){

                totalDigits = std::accumulate(fMapping.fStructsBuffer.begin(),fMapping.fStructsBuffer.end()-cutOut,0ull,lambdaSumDigits);
                nStrips = (uint64_t)std::count_if(fMapping.fStructsBuffer.begin(),fMapping.fStructsBuffer.end()-cutOut,lambdaIfNotZero);

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

            for( const auto &stripIterator : fMapping.fStructsBuffer ){
                uint64_t digitsCounter = (*stripIterator).digitsCounter[type];
//                LOG(DEBUG) << digitsCounter;
                if ( digitsCounter > meanCounts * 42 /* So long and thanks for all the fish */){
                    LOG(ERROR) << "Strip is noisy " << digitsCounter << " " << meanCounts;
                    stripIterator->isNoisy = true;
                }
//                else if ( digitsCounter < 0.01 * meanCounts ) (&*stripIterator)->isDead = true;
//                else {
//                    (*stripIterator).isNoisy = false;
//                    (*stripIterator).isDead = false;
//                }
            }

            previousColumnID = currentColumnID;

            fMapping.fStructsBuffer.clear();
        }

        fMapping.fStructsBuffer.emplace_back(strip);

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

    for( auto &vecIteratorRead : fMapping.fStripVector){
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

    for(const auto &mapIterator : fMapping.fIDMap){
        auto uniqueID = mapIterator.first;
        auto index = mapIterator.second;
        auto strip = &(fMapping.fStripVector[index]);

        if ( strip->isDead ) {

            auto alreadyThere = fStructMask.deadStripsIDs.insert(uniqueID).second;

            if(alreadyThere){
                LOG(ERROR)<<uniqueID<<" is dead.";
            } else {
//                LOG(INFO)<<uniqueID<<" already set.";
            }

        } else if ( strip->isNoisy ) {

            auto alreadyThere = fStructMask.noisyStripsIDs.insert(uniqueID).second;

            if(alreadyThere){
                LOG(ERROR)<<uniqueID<<" is noisy.";
            } else {
//                LOG(INFO)<<uniqueID<<" already set.";
            }

        }
//        else LOG(INFO)<<uniqueID<<" is working as expected.";
    }

    fStructMask.nDead = (UShort_t)fStructMask.deadStripsIDs.size();
    fStructMask.nNoisy = (UShort_t)fStructMask.noisyStripsIDs.size();

    return;
};

//_________________________________________________________________________________________________
errMsg MIDMaskGenerator::SendMask(){

    auto sum = fStructMask.nDead + fStructMask.nNoisy;

    if (sum==0) return kOk;

    int msgSize = sizeof(fStructMask.nDead) + sizeof(fStructMask.nNoisy) + sum * sizeof(IDType);
    FairMQMessagePtr msgOut(NewMessage(msgSize));

    auto header = reinterpret_cast<UShort_t*>(msgOut->GetData());
    header[0] = fStructMask.nDead;
    header[1] = fStructMask.nNoisy;
    auto payload = reinterpret_cast<IDType*>(&(header[2]));

    int position = 0;

    for( auto const &itDead : fStructMask.deadStripsIDs ){
        payload[position++] = itDead;
    }
    for( auto const &itNoisy : fStructMask.noisyStripsIDs ){
        payload[position++] = itNoisy;
    }

//    std::cout<< "Sending message" << std::endl;

    // Try to send the message. If unable trigger a error and abort killing the device
    if (SendAsync(msgOut, "mask-out") < 0) {
        return kFailedSend;
    }

    return kOk;
}