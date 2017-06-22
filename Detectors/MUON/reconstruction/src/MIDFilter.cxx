//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#include "MUONReconstruction/MIDFilter.h"
#include "MUONBase/Deserializer.h"
#include "FairMQLogger.h"
#include "options/FairMQProgOptions.h"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
MIDFilter::MIDFilter(){
    FairMQDevice::OnData("data-in", &MIDFilter::HandleData);
}

//_________________________________________________________________________________________________
MIDFilter::~MIDFilter(){

}

//_________________________________________________________________________________________________
MIDFilter::HandleData(FairMQMessagePtr&, int){

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

   LOG(INFO) << "Received valid message";

    while((uniqueIDBuffer = MessageDeserializer.NextUniqueID())){

        if ( ((*uniqueIDBuffer) & 0xFFF) < 1100 ) continue;

    }
}