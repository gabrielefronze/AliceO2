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

    LOG(INFO) << "Initializing device";

    fMapFilename = fConfig->GetValue<std::string>("binmapfile");

    if ( !(ReadMapping(fMapFilename.c_str())) ){
        LOG(ERROR) << "Error reading the mapping from " << fMapFilename;
    } else {
        LOG(INFO) << "Mapping correctly loaded.";
    }

}

//_________________________________________________________________________________________________
bool MIDoccupancy::HandleData( FairMQMessagePtr &msg, int /*index*/ )
{

    LOG(INFO) << "Received message";

    if ( !msg ) return false;

    Deserializer MessageDeserializer(msg);
    int counter = 0;

    Deserializer::deserializerDataStruct* deserializedData;

    if ( !deserializedData ) return false;

    while((deserializedData = MessageDeserializer.NextDigit())){
        counter++;
        LOG(INFO) << "Read "<<counter<<" data:";
        LOG(INFO) << "\t"<<deserializedData->fDetElemID;
        LOG(INFO) << "\t"<<deserializedData->fBoardID;
        LOG(INFO) << "\t"<<deserializedData->fChannel;
        LOG(INFO) << "\t"<<deserializedData->fCathode;
    }

    return true;
}

bool MIDoccupancy::ReadMapping( const char * filename )
{

    /// Read mapping

    auto tStart = std::chrono::high_resolution_clock::now();

    // std::vector<Mapping::mpDE> mpdeList = Mapping::ReadMapping(filename);
    int numberOfDetectionElements = 0;
    Mapping::mpDE* mpdeList = Mapping::ReadMapping(filename,numberOfDetectionElements);

    // if (mpdeList.size() == 0 ) {
    if ( numberOfDetectionElements == 0 ) {
        return false;
    }

    // for ( Mapping::mpDE mpde : mpdeList ) {
    for ( int impde=0; impde<numberOfDetectionElements; impde++ ) {

        // printf("mpDE id %i\n",mpde.id);
        Mapping::mpDE &mpde = mpdeList[impde];
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    LOG(INFO) << "Read mapping in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";

    return true;
}
