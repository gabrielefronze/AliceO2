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
#include "MUONBase/Serializer.h"
#include "MUONBase/Deserializer.h"

using namespace AliceO2::MUON;

BOOST_AUTO_TEST_SUITE(testDeserializer)

    BOOST_AUTO_TEST_CASE(UniqueID){
        Serializer serializer;
        serializer.AddDigit(4,3,2,1);

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(0) );
    }

    BOOST_AUTO_TEST_CASE(NextUniqueID){
        Serializer serializer;
        serializer.AddDigit(4,3,2,1);
        serializer.AddDigit(8,7,6,5);
        serializer.AddDigit(12,11,10,9);

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(0) );
        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(1) );
        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(2) );
    }

    BOOST_AUTO_TEST_CASE(CurrentUniqueID){
        Serializer serializer;
        serializer.AddDigit(4,3,2,1);

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(0) );
        BOOST_TEST( *(deserializer.CurrentUniqueID()) == serializer.GetUID(0) );
    }

    BOOST_AUTO_TEST_CASE(CurrentData){
        Serializer serializer;
        serializer.AddDigit(4,3,2,1);

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( deserializer.GetDataPointer()[0] == serializer.GetUID(0) );
        BOOST_TEST( deserializer.GetDataPointer()[1] == serializer.GetUID(0) );
    }

    BOOST_AUTO_TEST_CASE(Header){
        Serializer serializer;

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( deserializer.GetHeader()[0] == 0xDEAD );
        BOOST_TEST( deserializer.GetHeader()[100] == 0xDEAD );
        BOOST_TEST( deserializer.GetHeader()[1] == 0xBEEF );
        BOOST_TEST( deserializer.GetHeader()[99] == 0xBEEF );
    }

    BOOST_AUTO_TEST_CASE(NDigits){
        Serializer serializer;
        serializer.AddDigit(4,3,2,1);
        serializer.AddDigit(8,7,6,5);
        serializer.AddDigit(12,11,10,9);

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( deserializer.GetNDigits() == 3 );
    }

    BOOST_AUTO_TEST_CASE(Rewind){
        Serializer serializer;
        serializer.AddDigit(4,3,2,1);
        serializer.AddDigit(8,7,6,5);
        serializer.AddDigit(12,11,10,9);

        Deserializer deserializer(serializer.GetMessage());

        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(0) );
        auto dummy = deserializer.NextUniqueID();
        dummy = nullptr;
        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(2) );
        BOOST_TEST( deserializer.Rewind() );
        BOOST_TEST( *(deserializer.NextUniqueID()) == serializer.GetUID(0) );
    }

BOOST_AUTO_TEST_SUITE_END()

