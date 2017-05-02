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
MIDoccupancy::~MIDoccupancy() {
    if ( fInternalMapping ) delete fInternalMapping;
}

//_________________________________________________________________________________________________
void MIDoccupancy::InitTask() {

    fMapFilename = fConfig->GetValue<std::string>("binmapfile");

    int numberOfDetectionElements = 0;

    if ( !(fInternalMapping = Mapping::ReadMapping(fMapFilename.c_str(),numberOfDetectionElements)) ){
        LOG(ERROR) << "Error reading the mapping even if " << fMapFilename <<" exists.";
    } else {
        LOG(TRACE) << "Mapping correctly loaded with "<< numberOfDetectionElements <<" detector elements.";
    }

}

//_________________________________________________________________________________________________
bool MIDoccupancy::HandleData(FairMQMessagePtr &msg, int /*index*/)
{
    Deserializer MessageDeserializer(msg);
    int counter = 0;

    Deserializer::deserializerDataStruct* deserializedData;

    while((deserializedData = MessageDeserializer.NextDigit())){
        counter++;
        LOG(TRACE) << "Read "<<counter<<" data:";
        LOG(TRACE) << "\t"<<deserializedData->fDetElemID;
        LOG(TRACE) << "\t"<<deserializedData->fBoardID;
        LOG(TRACE) << "\t"<<deserializedData->fChannel;
        LOG(TRACE) << "\t"<<deserializedData->fCathode;
    }
}
