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
    for (uint32_t iHeader = 0; iHeader < kHeaderLength; ++iHeader) {
        fHeader.push_back(iHeader);
    }
}

//_________________________________________________________________________________________________
uint32_t* Serializer::GetMessage() {

    std::vector<uint32_t>().swap(fOutputData);

    // The first 100bytes are the header.
    fOutputData.assign(fHeader.begin(), fHeader.begin() + kHeaderLength);

//    for( const auto &itData : OutputData ){
//        std::cout<<itData<<std::endl;
//    }

    // Putting in the vector the number of digits before the payload
    fOutputData.emplace_back(fData.size()/2);

    // Putting in OutputData the translated structs
    for( const auto &itData : fData ){
        fOutputData.emplace_back(GetUID(itData));
    }

    return &(fOutputData[0]);
}

//_________________________________________________________________________________________________
uint32_t Serializer::GetUID(deserializerDataStruct dataStruct) {
    uint32_t digitBuffer = 0;

    digitBuffer |= (dataStruct.fDetElemID & 0xFFF);
    digitBuffer |= (dataStruct.fBoardID & 0xFFF) << 12; //shift of 12 bits
    digitBuffer |= (dataStruct.fChannel & 0x3F) << 24; //shift of 24 bits
    digitBuffer |= (dataStruct.fCathode & 0x4) << 30; //shift of 30 bits

    return digitBuffer;
}

//_________________________________________________________________________________________________
uint32_t Serializer::GetUID(size_t index) {

    auto index2 = index*2;

    assert(index2 < fData.size());

    uint32_t digitBuffer = 0;

    digitBuffer |= (fData[index2].fDetElemID & 0xFFF);
    digitBuffer |= (fData[index2].fBoardID & 0xFFF) << 12; //shift of 12 bits
    digitBuffer |= (fData[index2].fChannel & 0x3F) << 24; //shift of 24 bits
    digitBuffer |= (fData[index2].fCathode & 0x4) << 30; //shift of 30 bits

    return digitBuffer;
}
