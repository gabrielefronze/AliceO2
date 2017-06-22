//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#include "MUONReconstruction/MIDFilter.h"
#include "MUONBase/Deserializer.h"
#include "FairMQLogger.h"
#include "options/FairMQProgOptions.h"
#include "flatbuffers/flatbuffers.h"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
MIDFilter::MIDFilter(){
    FairMQDevice::OnData("mask-in", &MIDFilter::HandleMask);
    FairMQDevice::OnData("data-in", &MIDFilter::HandleData);
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

    LOG(INFO) << "Received valid message";

    Deserializer MessageDeserializer(msg);

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

        // check if the ID is present in the noisy or dead strips' set
        auto IsStripOk =   (fMask.noisyStripsIDs.find(*uniqueIDBuffer)    == fMask.noisyStripsIDs.end()
                            && fMask.deadStripsIDs.find(*uniqueIDBuffer)  == fMask.deadStripsIDs.end());

        // If the ID corresponds to a strip which is behaving well save it in the vector
        if ( IsStripOk ){
            uint32_t* Data = MessageDeserializer.GetCurrentData();
            OutputDataDigits.push_back(Data[0]);
            OutputDataDigits.push_back(Data[1]);
            nDigits++;
        }
    }

    // The first element after the header of the reacreated message should be the NEW number of digits
    OutputData.push_back(nDigits);

    // Then all the digits can follow
    OutputData.assign(OutputDataDigits.begin(),OutputDataDigits.end());
}

bool MIDFilter::HandleMask( FairMQMessagePtr &msg, int /*index*/ ) {

    // TODO
    return true;
}