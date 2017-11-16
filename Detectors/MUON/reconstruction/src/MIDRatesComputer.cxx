//
// Created by Gabriele Gaetano Fronzé on 26/06/2017.
//

#include <TRandom.h>
#include <TMath.h>
#include <FairLogger.h>
#include "MUONReconstruction/MIDRatesComputer.h"
#include "MUONBase/Deserializer.h"
#include "FairMQLogger.h"
#include "options/FairMQProgOptions.h"
#include "algorithm"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
MIDRatesComputer::MIDRatesComputer():
        FairMQDevice()
{
    // The input of this device is the digits stream
    FairMQDevice::OnData("data-in", &MIDRatesComputer::HandleData);
}

//_________________________________________________________________________________________________
MIDRatesComputer::~MIDRatesComputer() {
    // Output of simulated noisy strips
    LOG(DEBUG) << "Simulated noisy strips:";
    for(const auto &itMask : fStructMaskSim.noisyStripsIDs){
        LOG(DEBUG) << "\t" << itMask << "\t\t" << fMapping[itMask]->digitsCounter[digitType::kPhysics];
    }
}

//_________________________________________________________________________________________________
void MIDRatesComputer::InitTask() {

    LOG(INFO) << "Initializing device";

    // Loading mapping at startup
    std::string mapFilename = fConfig->GetValue<std::string>("binmapfile");
    if ( !(fMapping.ReadMapping(mapFilename.c_str())) ){
        LOG(ERROR) << "Error reading the mapping from " << mapFilename;
    } else {
        LOG(INFO) << "Mapping correctly loaded: " << fMapping.Consistent(true);
    }
}

//_________________________________________________________________________________________________
bool MIDRatesComputer::HandleData( FairMQMessagePtr &msg, int /*index*/ )
{

    // If the message is empty something is going wrong. The process should be aborted.
    if ( !msg ) {
        LOG(ERROR) << "Message pointer not valid, aborting";
        return false;
    }

    // If the input is smaller than the header size the message is empty and we should skip.
    if ( msg->GetSize()<100 ) {
        LOG(ERROR) << "Message empty, skipping";
        return true;
    }

//    LOG(INFO) << msg->GetSize();

    // Deserializer will help decoding the message
    Deserializer MessageDeserializer(msg);

    // Counter of received digits
    int counter = 0;

    LOG(INFO) << "Received valid message with " << MessageDeserializer.GetNDigits() <<" digits";

    // Loop over the digits of the message.
    uint32_t *uniqueIDBuffer;
    while((uniqueIDBuffer = MessageDeserializer.NextUniqueID())){
//        LOG(INFO) << "UniqueID "<<  ((*uniqueIDBuffer) & 0xFFF);

        // We want to discard MCH digits (if any)
        if ( ((*uniqueIDBuffer) & 0xFFF) < 1100 ) continue;

        counter++;

        digitType digitType = digitType::kSize;

        // Try to retrieve a pointer to the data member to modify
        stripMapping* strip = fMapping[*uniqueIDBuffer];
        if(!strip) {
            LOG(ERROR) << "Mapping not found for that uniqueID: " << uniqueIDBuffer;
            continue;
        }


        // TODO: here we need to check the kind of event. No clue on where it is stored for the moment.
        if ( true /*physics*/ ){
            digitType = digitType::kPhysics;
        } else if ( false /*FET*/ ){
            digitType = digitType::kFET;
        } else if ( false /*Triggered*/ ){
            digitType = digitType::kTriggered;
        } else continue;

        LOG(INFO) << "StripMapping struct found for element: " << MessageDeserializer.PrintData() << " " << strip->digitsCounter[0];

        // Increase the counter of digits for the strip
        strip->digitsCounter[digitType]++;
        LOG(INFO) << (long int)strip->digitsCounter[digitType];
//        if ( gRandom->Rndm() > 0.99 ){
//            strip->digitsCounter[digitType]+=999999;
//            LOG(ERROR) << "Simulating noisy strip " << ((*uniqueIDBuffer) & 0xFFF);
//            auto dummy = fStructMaskSim.noisyStripsIDs.insert(*uniqueIDBuffer).second;
//            fStructMaskSim.nNoisy++;
//        }
    }

    LOG(INFO) << "Received valid message containing "<<counter<<" digits";

    // Check if enough statistics is already present for a given digitType
    if ( !EnoughStatistics(digitType::kPhysics) ) {
        LOG(INFO) << "Not enough statistics: waiting for more.";
        return true;
    };

    LOG(INFO) << "Computing rates.";
    // If enough statistics compute all rates
    MIDRatesComputer::ComputeAllRates();

    // Try to send newly computed rates and catch errors
    switch (MIDRatesComputer::SendRates<uint64_t>()) {
        case kShortMsg:
            LOG(ERROR) << "Message shorter than expected. Skipping.";
            return true;

        case kFailedSend:
            LOG(ERROR) << "Problems sending rates. Aborting.";
            return false;

        case kOk:
            return true;

        default:
            return true;
    }
}

//_________________________________________________________________________________________________
void MIDRatesComputer::ResetCounters(uint64_t newStartTS, digitType type) {

    auto tStart = std::chrono::high_resolution_clock::now();

    //  Reset all counters and timestamps
    for( auto &vecIterator : fMapping.fStripVector){
        stripMapping* strip = &vecIterator;
        strip->digitsCounter[type]=0;
        strip->startTS[type]=newStartTS;
        strip->stopTS[type]=0;
        strip->isNoisy = false;
        strip->isDead = false;
        strip->rate[type] = 0;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

//    LOG(DEBUG) << "Reset counters in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

//_________________________________________________________________________________________________
bool MIDRatesComputer::EnoughStatistics(digitType type) {
    // Return if enough statistics has been collected. Customizable.
    long nOfActiveStrips = std::count_if(fMapping.fStripVector.begin(),fMapping.fStripVector.end(),[type](stripMapping strip)->bool{ return strip.digitsCounter[type] > 10; });
    return nOfActiveStrips > (0.001 * fMapping.fStripVector.size());
}

//_________________________________________________________________________________________________
void MIDRatesComputer::ComputeRate(stripMapping* strip) {

    // Compute all the rates for a given strip
    for (int iType = 0; iType < digitType::kSize; ++iType) {
        strip->rate[iType] = (float_t)(strip->digitsCounter[iType]) / strip->area;

        uint64_t startTS = strip->startTS[iType];
        uint64_t stopTS = strip->stopTS[iType];

        if ( stopTS > startTS ){
            strip->rate[iType]/=(stopTS-startTS);
        }
    }
}

//_________________________________________________________________________________________________
void MIDRatesComputer::ComputeAllRates() {

    auto tStart = std::chrono::high_resolution_clock::now();

    // Compute all rates
    for(auto &stripIt : fMapping.fStripVector){
        ComputeRate(&stripIt);
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

//    LOG(DEBUG) << "Rates computed in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";
}

//_________________________________________________________________________________________________
template<typename T> errMsg MIDRatesComputer::SendRates(){
    auto tStart = std::chrono::high_resolution_clock::now();

    // Message size is kSize T elements for each strip
    uint64_t msgSize = fMapping.fStripVector.size() * digitType::kSize;

//    LOG(DEBUG) << "Msgsize is " << msgSize;

    // Instance message as unique pointer
    FairMQMessagePtr msgOut(NewMessage((int)(msgSize* sizeof(T))));

    // Pointer to message payload
    T *dataPointer = reinterpret_cast<T*>(msgOut->GetData());

    // Copy OutputData in the payload of the message
    for ( int iData = 0; iData < fMapping.fStripVector.size(); iData++ ) {
        for (int iType = 0; iType < digitType::kSize; iType++ ) {
            dataPointer[iData * 3 + iType] = fMapping.fStripVector[iData].digitsCounter[iType];
        }
    }

    // Try to send the message. If unable trigger a error and abort killing the device
    auto status = Send(msgOut, "rates-out");

//    LOG(DEBUG) << "Send sent " << status << " bits";

    if ( status < 0) {
        return kFailedSend;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

//    LOG(DEBUG) << "Rates sent in " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms";

    return kOk;
}