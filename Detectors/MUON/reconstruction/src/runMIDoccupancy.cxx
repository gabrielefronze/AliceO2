//
// Created by Gabriele Gaetano Fronzé on 02/05/2017.
//

#include "runFairMQDevice.h"
#include "MUONReconstruction/MIDoccupancy.h"

namespace bpo = boost::program_options;
using namespace std;

void addCustomOptions(bpo::options_description& options)
{
  options.add_options()
  // ("help,h", "produces this usage message")
  ("source,s", bpo::value<std::string>(), "address to get the messages from")
  ("binmapfile", bpo::value<std::string>(), "file with segmentation")
  ;
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
  return new AliceO2::MUON::MIDoccupancy();
}
