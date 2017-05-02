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
    fInternalMapping = 0x0;
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

    if ( !msg ) {
        LOG(ERROR) << "Message pointer not valid, aborting";
        return false;
    }

    if ( msg->GetSize()<100 ) {
        LOG(ERROR) << "Message empty, skipping";
        return true;
    }

    Deserializer MessageDeserializer(msg);
    int counter = 0;

    Deserializer::deserializerDataStruct* deserializedData;

    while((deserializedData = MessageDeserializer.NextDigit())){
        counter++;
//        LOG(INFO) << "\t"<<deserializedData->fDetElemID;
//        LOG(INFO) << "\t"<<deserializedData->fBoardID;
//        LOG(INFO) << "\t"<<deserializedData->fChannel;
//        LOG(INFO) << "\t"<<deserializedData->fCathode;
    }

    LOG(INFO) << "Received valid message containing "<<counter<<" digits";
    return true;
}

//_________________________________________________________________________________________________
bool MIDoccupancy::ReadMapping( const char * filename )
{
    /// Read mapping

    auto tStart = std::chrono::high_resolution_clock::now();

    // std::vector<Mapping::mpDE> mpdeList = Mapping::ReadMapping(filename);
    int numberOfDetectionElements = 0;
    fInternalMapping = Mapping::ReadMapping(filename,numberOfDetectionElements);

    auto tEnd = std::chrono::high_resolution_clock::now();
    LOG(INFO) << "Mapping loaded in: " << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";

    return true;
}
