//
// Created by Gabriele Gaetano Fronzé on 12/04/2018.
//

#ifndef ALICEO2_UTILS_H
#define ALICEO2_UTILS_H

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file Utils.h
/// \brief A collection of , v0.1
///
/// \author Gabriele Gaetano Fronzé, gfronze@cern.ch

#include "Framework/DataProcessorSpec.h"

namespace o2f = o2::framework;

namespace o2 {
namespace workflows {
o2f::DataProcessorSpec defineBroadcaster(std::string devName, o2f::InputSpec usrInput, o2f::Outputs usrOutputs,
                                         std::function<size_t(o2f::DataRef)> const& func);
o2f::DataProcessorSpec defineBroadcaster(std::string devName, o2f::InputSpec usrInput, o2f::Outputs usrOutputs,
                                         size_t fixMsgSize);
}
}

#endif //ALICEO2_UTILS_H
