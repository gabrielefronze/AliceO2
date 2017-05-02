//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#include "MUONReconstruction/MIDoccupancy.h"
#include "FairMQLogger.h"
#include "options/FairMQProgOptions.h"
#include "boost"

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
MIDoccupancy::MIDoccupancy(TString mapFilename):
FairMQDevice(),
fMessageDeserializer(),
fInternalMapping(0x0),
fMapFilename(mapFilename){}

//_________________________________________________________________________________________________
MIDoccupancy::~MIDoccupancy() {
    if ( fInternalMapping ) delete fInternalMapping;
}

//_________________________________________________________________________________________________
void MIDoccupancy::InitTask() {

    if ( !boost::filesystem::exists(fMapFilename.Data()) ){
        LOG(ERROR) << "Could not read binary mapping file: " << fMapFilename.Data();
        return;
    } else {
        LOG(TRACE) << "File " << fMapFilename.Data()<<" found.";
    }

    int numberOfDetectionElements = 0;
    fInternalMapping = Mapping::ReadMapping(fMapFilename.Data(),numberOfDetectionElements);

    if ( !fInternalMapping ){
        LOG(ERROR) << "Error reading the mapping even if " << fMapFilename.Data()<<" exists.";
    } else {
        LOG(TRACE) << "Mapping correctly loaded with "<<numberOfDetectionElements<<" detector elements.";
    }

}

//_________________________________________________________________________________________________
bool MIDoccupancy::HandleData(FairMQMessagePtr &msg, int /*index*/)
{
    fMessageDeserializer = Deserializer(msg);
    int counter = 0;

    Deserializer::deserializerDataStruct* deserializedData;

    while(deserializedData = fMessageDeserializer.NextDigit()){
        counter++;
        LOG(TRACE) << "Read "<<counter<<" data:";
        LOG(TRACE) << "\t"<<deserializedData->fDetElemID;
        LOG(TRACE) << "\t"<<deserializedData->fBoardID;
        LOG(TRACE) << "\t"<<deserializedData->fChannel;
        LOG(TRACE) << "\t"<<deserializedData->fCathode;
    }
}
