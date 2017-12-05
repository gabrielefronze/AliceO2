#ifndef O2_DEV_ALO_MIDRATESCOMPUTER_H
#define O2_DEV_ALO_MIDRATESCOMPUTER_H

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
///  @file   MIDRatesComputer
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Device to compute rates based on occupancy information for MID
///

#include "FairMQDevice.h"
#include "MUONBase/Enums.h"
#include "MUONBase/OccupancyMapping.h"
#include "MUONBase/Mapping.h"
#include "MUONBase/Chrono.h"
#include "string.h"
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>


namespace o2 {

    namespace muon {

        namespace mid {

            class MIDRatesComputerDevice : public FairMQDevice {
            public:
                MIDRatesComputerDevice();

                virtual ~MIDRatesComputerDevice();

            protected:
                bool HandleData(FairMQMessagePtr &, int);

                virtual void InitTask();

            private:

                OccupancyMapping fMapping;

                struct stripMask {
                    ushort_t nDead; // number of elements for deadStripsIDs
                    ushort_t nNoisy; // number of elements for noisyStripsIDs
                    std::unordered_set<uint32_t> deadStripsIDs; // container of UniqueIDs of dead strips
                    std::unordered_set<uint32_t> noisyStripsIDs; // container of UniqueIDs of noisy strips
                };

                stripMask fStructMaskSim;

                void ResetCounters(uint64_t newStartTS, digitType type);

                bool ShouldComputeRates(digitType type);

                void ComputeRate(stripMapping *strip);

                void ComputeAllRates();

                template<typename T>
                errMsg SendRates();

                long fCounter;

                //Chrono object to compute duration
                Chrono fChronometer;
            };
        }
    }
}


#endif //O2_DEV_ALO_MIDRATESCOMPUTER_H
