#ifndef DESERIALIZER_H
#define DESERIALIZER_H

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
///  @brief  Little deserializer app to unpack MID messages
///

#include "FairMQMessage.h"

namespace AliceO2 {

  namespace MUON {

    class Deserializer {

    public:

        Deserializer();
        explicit Deserializer(FairMQMessagePtr& msg);
        explicit Deserializer(void *payload);
        ~Deserializer();

        // Human readable data struct for output
        struct deserializerDataStruct {
            uint32_t fDetElemID;
            uint32_t fBoardID;
            uint32_t fChannel;
            uint32_t fCathode;
        };

        // Iterator like methods
        bool Advance();
        void Load();
        bool Rewind();
        deserializerDataStruct* NextDigit();
        inline deserializerDataStruct* CurrentDigit(){ return &fOutputDataStruct; }
        inline deserializerDataStruct* operator() (){ return NextDigit(); }

        uint32_t * NextUniqueID(bool loadAllData = false);
        inline uint32_t* CurrentUniqueID(){ return &fUniqueID; }
        inline uint32_t* GetCurrentData(){ return fData; }
        inline uint32_t* GetHeader(){ return reinterpret_cast<uint32_t(&)[25]>(reinterpret_cast<uint8_t(&)[100]>(fDataPtr)); }
        inline uint32_t* GetDataPointer(){ return fDigitsDataPtr; }
        inline uint32_t GetNDigits(){ return fNDigits; }
        std::string PrintData(){
            std::string outputString =  "UID="+std::to_string(fUniqueID)+
                                       " ElemID="+std::to_string(fOutputDataStruct.fDetElemID)+
                                       " Bd="+std::to_string(fOutputDataStruct.fBoardID)+
                                       " Ch="+std::to_string(fOutputDataStruct.fChannel)+
                                       " Cat="+std::to_string(fOutputDataStruct.fCathode);
            return outputString;
        };

    private:
        // Const values to exclude first 100 bytes of message and perform deserialization
        const uint32_t kHeaderLength = 100;

        uint8_t* fDataPtr;
        uint32_t* fDigitsDataPtr;
        uint32_t fNDigits;
        uint32_t fOffset;
        uint32_t fDigitCounter;
        uint32_t fUniqueID;
        uint32_t fData[2];

        // Internal data container
        deserializerDataStruct fOutputDataStruct;

    };

  }

}

#endif //DESERIALIZER_H
