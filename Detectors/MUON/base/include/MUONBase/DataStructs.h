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
///  @file   DataStructs
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Container for usefull data structs
///

#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

// Human readable data struct for output
struct deserializerDataStruct {
    uint32_t fDetElemID;
    uint32_t fBoardID;
    uint32_t fChannel;
    uint32_t fCathode;
    deserializerDataStruct(uint32_t detElemID, uint32_t boardID, uint32_t channel, uint32_t cathode){
        fDetElemID = detElemID;
        fBoardID = boardID;
        fChannel = channel;
        fCathode = cathode;
    }
};

#endif //DATASTRUCTS_H
