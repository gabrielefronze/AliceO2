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
///  @file   Deserializer
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Little deserializer app to pack MID messages
///

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <vector>
#include "DataStructs.h"

namespace AliceO2 {

    namespace MUON {

        class Serializer {

        public:

            Serializer();
            ~Serializer(){};

            inline void AddDigit(uint32_t detElemID, uint32_t boardID, uint32_t channel, uint32_t cathode){ fData.emplace_back(detElemID,boardID,channel,cathode);fData.emplace_back(detElemID,boardID,channel,cathode); };
            inline void AddDigit(deserializerDataStruct dataStruct){ fData.emplace_back(dataStruct);fData.emplace_back(dataStruct); };
            uint32_t GetUID(deserializerDataStruct dataStruct);
            uint32_t GetUID(size_t index);
            uint32_t* GetMessage();


        private:
            // Const values to exclude first 100 bytes of message and perform deserialization
            const uint32_t kHeaderLength = 100;
            uint32_t fHeader[25];

            // Internal data container
            std::vector<deserializerDataStruct> fData;

        };

    }

}


#endif //SERIALIZER_H
