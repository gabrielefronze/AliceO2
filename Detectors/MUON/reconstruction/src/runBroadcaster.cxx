#include "runFairMQDevice.h"
#include "MUONReconstruction/Broadcaster.h"

namespace bpo = boost::program_options;
using namespace std;

void addCustomOptions(bpo::options_description& options)
{
  options.add_options();
}

FairMQDevicePtr getDevice(const FairMQProgOptions& /*config*/)
{
  return new AliceO2::MUON::Broadcaster();
}