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
#include "MUONBase/Chrono.h"

BOOST_AUTO_TEST_SUITE(Chronotest)

    BOOST_AUTO_TEST_CASE(Average){
        Chrono chronometer;

        int nCalls = 10;
        int sum = 0;
        for (int jCall = 0; jCall < nCalls; ++jCall) {
            chronometer.AddCall((double)jCall);
            sum += jCall;
        }

        double avg = (double)sum/(double)nCalls;

        BOOST_TEST( avg == chronometer.AvgCallTime() );
    }

    BOOST_AUTO_TEST_CASE(PrintStatus){
        Chrono chronometer;

        int nCalls = 10;
        int sum = 0;
        for (int jCall = 0; jCall < nCalls; ++jCall) {
            chronometer.AddCall((double)jCall);
            sum += jCall;
        }

        double avg = (double)sum/(double)nCalls;

        auto str = chronometer.PrintStatus();
        int strNCalls = 0;
        double strAvg = 0.;
        scanf(str.c_str(),"NCalls=%d AvgTime=%lf",&strNCalls,&strAvg);

        BOOST_TEST( avg == strAvg );
        BOOST_TEST( strNCalls == nCalls );
    }

BOOST_AUTO_TEST_SUITE_END()

