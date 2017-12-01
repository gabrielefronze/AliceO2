//
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @author  Gabriele Gaetano Fronzé

#include "MUONBase/Serializer.h"
#include <assert.h>
#include <regex.h>

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
Serializer::Serializer() {
    for (int iHeader = 0; iHeader < kHeaderLength/4; ++iHeader) {
        fHeader[iHeader] = (iHeader%2==0)?0xDEAD:0xBEEF;
    }
}

//_________________________________________________________________________________________________
uint32_t* Serializer::GetMessage() {
    // This vector will contain the full message
    std::vector<uint32_t> OutputData;

    // The first 100bytes are the header.
    OutputData.assign(fHeader, fHeader + 25);

    // Putting in the vector the number of digits before the payload
    OutputData.emplace_back(fData.size());

    // Putting in OutputData the translated structs
    for( const auto &itData : fData ){
        OutputData.emplace_back(GetUID(itData));
        OutputData.emplace_back(GetUID(itData));
    }

    return &(OutputData[0]);
}

//_________________________________________________________________________________________________
uint32_t Serializer::GetUID(deserializerDataStruct dataStruct) {
    uint32_t digitBuffer = 0;

    digitBuffer |= dataStruct.fDetElemID;
    digitBuffer |= dataStruct.fBoardID << 12; //shift of 12 bits
    digitBuffer |= dataStruct.fChannel << 24; //shift of 24 bits
    digitBuffer |= dataStruct.fCathode << 30; //shift of 30 bits

    return digitBuffer;
}

//_________________________________________________________________________________________________
uint32_t Serializer::GetUID(size_t index) {

    auto index2 = index*2;

    assert(index2 < fData.size());

    uint32_t digitBuffer = 0;

    digitBuffer |= fData[index2].fDetElemID;
    digitBuffer |= fData[index2].fBoardID << 12; //shift of 12 bits
    digitBuffer |= fData[index2].fChannel << 24; //shift of 24 bits
    digitBuffer |= fData[index2].fCathode << 30; //shift of 30 bits

    return digitBuffer;
}
