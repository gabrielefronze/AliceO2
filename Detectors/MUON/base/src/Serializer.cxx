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

using namespace AliceO2::MUON;

//_________________________________________________________________________________________________
Serializer::Serializer() {
    for (int iHeader = 0; iHeader < kHeaderLength; ++iHeader) {
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
    uint32_t digitBuffer = 0;
    for( const auto &itData : fData ){
        digitBuffer = 0;

        digitBuffer |= itData.fDetElemID;
        digitBuffer |= itData.fBoardID << 12; //shift of 12 bits
        digitBuffer |= itData.fChannel << 24; //shift of 24 bits
        digitBuffer |= itData.fCathode << 30; //shift of 30 bits

        OutputData.emplace_back(digitBuffer);
    }

    return &(OutputData[0]);
}
