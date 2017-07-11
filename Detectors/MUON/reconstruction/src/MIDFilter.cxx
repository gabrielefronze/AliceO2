//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#include "MUONReconstruction/MIDFilter.h"
#include "MUONBase/Deserializer.h"
#include "FairMQLogger.h"
#include "options/FairMQProgOptions.h"
//#include "flatbuffers/flatbuffers.h"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
MIDFilter::MIDFilter(){

    fMask.nDead = 0;
    fMask.nNoisy = 0;
    fMask.deadStripsIDs.clear();
    fMask.noisyStripsIDs.clear();

    FairMQDevice::OnData("mask-in", &MIDFilter::HandleMask);
    FairMQDevice::OnData("digits-in", &MIDFilter::HandleData);
}

//_________________________________________________________________________________________________
MIDFilter::~MIDFilter(){

}

//_________________________________________________________________________________________________
bool MIDFilter::HandleData( FairMQMessagePtr &msg, int /*index*/ ){

    if ( !msg ) {
        LOG(ERROR) << "Message pointer not valid, aborting";
        return false;
    }

    if ( msg->GetSize()<100 ) {
//        LOG(ERROR) << "Message empty, skipping";
        return true;
    }

    // Deserializer will simplify the reading of the input message
    Deserializer MessageDeserializer(msg);

    LOG(INFO) << "Received valid message containing " << MessageDeserializer.GetNDigits() << " digits";

    return true;

    // Check if no noisy strip is found. If none simply forward the message
    if ( fMask.nNoisy == 0 ) {

        auto returnValue = (SendAsync(msg, "digits-out") < 0);

        if (returnValue) LOG(ERROR) << "Problems forwarding digits. Aborting.";

        return !returnValue;
    }

    LOG(DEBUG) << "Processing message";

    // Getting the header as 32bit integer pointer (instead of 8bit) to push it back in output message
    uint32_t* DataHeader = MessageDeserializer.GetHeader();

    // This vector will contain the full message
    std::vector<uint32_t> OutputData;

    // The first 100bytes are the header. It should remain the same.
    OutputData.assign(DataHeader[0], DataHeader[0] + 25);

    // This container will contain the digits from non problematic strips
    std::vector<uint32_t> OutputDataDigits;

    // Buffer for the unique ID
    uint32_t *uniqueIDBuffer;

    // Keeping track of the digits to be re-added to the output message.
    uint32_t nDigits = 0;

    // Loop over digits
    while((uniqueIDBuffer = MessageDeserializer.NextUniqueID())){

        // check if the ID is present in the noisy strips' set
        auto IsStripOk = fMask.noisyStripsIDs.find(*uniqueIDBuffer) == fMask.noisyStripsIDs.end();

        // If the ID corresponds to a strip which is behaving well save it in the vector
        if ( IsStripOk ){
            uint32_t* Data = MessageDeserializer.GetCurrentData();
            OutputDataDigits.push_back(Data[0]);
            OutputDataDigits.push_back(Data[1]);
            nDigits++;
        }
    }

    LOG(DEBUG) << "Ready to send";

    // The first element after the header of the reacreated message should be the NEW number of digits
    OutputData.push_back(nDigits);

    // Then all the digits can follow
    OutputData.assign(OutputDataDigits.begin(),OutputDataDigits.end());

    // Using templated function and creating meaningful output
    switch (SendMsg(OutputData.size(), &OutputData[0])) {
        case kShortMsg:
            LOG(ERROR) << "Message shorter than expected. Skipping.";
            return true;

        case kFailedSend:
            LOG(ERROR) << "Problems sending masked digits. Aborting.";
            return false;

        case kOk:
            return true;

        default:
            return true;
    }
}

//_________________________________________________________________________________________________
bool MIDFilter::HandleMask( FairMQMessagePtr &msg, int /*index*/ ) {\

    LOG(DEBUG) << "Mask has been received";

    // Clearing the mask data. The new mask is a complete information (not a diff).
    fMask.nDead = 0;
    fMask.nNoisy = 0;
    fMask.deadStripsIDs.clear();
    fMask.noisyStripsIDs.clear();

    // The header is made of two UShort_t which are counters of the number of dead and noisy strips respectively
    UShort_t* maskHeader = reinterpret_cast<UShort_t*>(msg->GetData());

    // If the received message has no problematic strip just leave the mask empty
    if ( maskHeader[0]==0 && maskHeader[1]==0 ){
        LOG(DEBUG) << "Received empty mask.";
        return true;
    }

    // Load unique IDs in maskData
    uint32_t* maskData = reinterpret_cast<uint32_t*>(&(maskHeader[2]));

    // Load the number of dead and noisy strips in the mask
    fMask.nDead = maskHeader[0];
    fMask.nNoisy = maskHeader[1];

    // Load the unique IDs in the mask object
    if(fMask.nDead>0)fMask.deadStripsIDs = std::unordered_set<uint32_t>(&(maskData[0]),&(maskData[fMask.nDead-1]));
    if(fMask.nNoisy>0)fMask.noisyStripsIDs = std::unordered_set<uint32_t>(&(maskData[fMask.nDead]),&(maskData[fMask.nDead+fMask.nNoisy-1]));

    LOG(DEBUG) << "Mask correctly loaded with " << fMask.nDead + fMask.nNoisy << " problematic strips";

    return true;
}

//_________________________________________________________________________________________________
template<typename T> errMsg MIDFilter::SendMsg(uint64_t msgSize, T* data){
    // Create unique pointer to a message of the right size
    FairMQMessagePtr msgOut(NewMessage((int)(msgSize * sizeof(T))));

    // Cast the pointer to the message payload to std::vector pointer to simplify copy
    T *dataPointer = reinterpret_cast<T *>(msgOut->GetData());

    int counter = 0;

    // Copy OutputData in the payload of the message
    for ( int iData = 0; iData < msgSize; iData++ ) {
        dataPointer[iData] = data[iData];
        counter++;
    }

    // Just a check that will be deleted after some testing
    if ( counter < msgSize-1 ) {
        return kShortMsg;
    }

//    std::cout<< "Sending message" << std::endl;

    // Try to send the message. If unable trigger a error and abort killing the device
    if (SendAsync(msgOut, "digits-out") < 0) {
        return kFailedSend;
    }

    return kOk;
}