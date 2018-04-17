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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "MIDBase/FilteringSerializer.h"
#include "MIDBase/Deserializer.h"

using namespace o2::mid;

BOOST_AUTO_TEST_SUITE(testDeserializer)

    BOOST_AUTO_TEST_CASE(UniqueID){
        FilteringSerializer serializer;
        serializer.AddDigit(0,0,0,0);

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(0) );
    }

    BOOST_AUTO_TEST_CASE(NextUniqueID){
        FilteringSerializer serializer;

        auto nDigits = 10;

        for (uint32_t iDigit = 0; iDigit < nDigits; ++iDigit) {
            serializer.AddDigit(iDigit,iDigit,iDigit,iDigit);
        }

        Deserializer deserializer(serializer.GetMessage());

        for (int iDigit = 0; iDigit < nDigits; ++iDigit) {
            BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(iDigit) );
        }
    }

    BOOST_AUTO_TEST_CASE(CurrentUniqueID){
        FilteringSerializer serializer;
        serializer.AddDigit(3,3,3,3);

        Deserializer deserializer(serializer.GetMessage());

        deserializer.NextUniqueID();
        BOOST_TEST( *(deserializer.CurrentUniqueID()) == serializer.GetUID(0) );
    }

    BOOST_AUTO_TEST_CASE(Header){
        FilteringSerializer serializer;

//        serializer.DumpHeader();

        Deserializer deserializer(serializer.GetMessage());

        auto header = deserializer.GetHeader();

        for (int iHeader = 0; iHeader < 25; ++iHeader) {
            BOOST_TEST( header[iHeader] == iHeader );
        }
    }

    BOOST_AUTO_TEST_CASE(NDigits){
        FilteringSerializer serializer;
        serializer.AddDigit(3,3,3,3);
        serializer.AddDigit(4,4,4,4);
        serializer.AddDigit(5,5,5,5);

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( deserializer.GetNDigits() == 3 );
    }

    BOOST_AUTO_TEST_CASE(Rewind){
        FilteringSerializer serializer;
        serializer.AddDigit(3,3,3,3);
        serializer.AddDigit(4,4,4,4);
        serializer.AddDigit(5,5,5,5);

        Deserializer deserializer(serializer.GetMessage());

        auto firstDigit = *(deserializer.NextUniqueID());
        deserializer.NextUniqueID();
        deserializer.NextUniqueID();
        BOOST_TEST( deserializer.Rewind() );
        BOOST_TEST( *(deserializer.NextUniqueID()) == firstDigit );
    }

BOOST_AUTO_TEST_SUITE_END()

