//
// Created by Gabriele Gaetano Fronzé on 06/07/2017.
//

#include "runFairMQDevice.h"
#include "MUONReconstruction/MIDMaskGenerator.h"

namespace bpo = boost::program_options;
using namespace std;

void addCustomOptions(bpo::options_description& options)
{
  options.add_options()
  // ("help,h", "produces this usage message")
  ("binmapfile", bpo::value<std::string>(), "file with segmentation")
  ;
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
  return new AliceO2::MUON::MIDMaskGenerator();
}