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

#include "MIDFilterDevice.h"
#include "runFairMQDevice.h"

namespace bpo = boost::program_options;
using namespace std;

void addCustomOptions(bpo::options_description& options)
{
  options.add_options()
    // ("help,h", "produces this usage message")
    //  ("source,s", bpo::value<std::string>(), "address to get the messages from")
    ("binmapfile", bpo::value<std::string>(), "file with segmentation");
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/) { return new o2::mid::MIDFilterDevice(); }