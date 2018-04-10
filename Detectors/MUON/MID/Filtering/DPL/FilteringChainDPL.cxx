//
// Created by Gabriele Gaetano Fronzé on 10/04/2018.
//

#include "FilteringChainDPL.h"
#include <MUONReconstruction/MIDRatesComputerAlgorithm.h>
#include <MUONBase/Serializer.h>
#include "random"

namespace o2f = o2::framework;

o2f::Inputs noInputs{};
o2f::Outputs noOutputs{};

o2f::DataProcessorSpec defineBroadcaster(uint32_t detElemID){
  return {
    "Broadcaster", //Device name
    noInputs, //No inputs, for the moment
    o2f::Outputs{{"MID","DIGITS",detElemID,o2f::OutputSpec::Lifetime::Timeframe}}, //Outputs are digits (aka DetElemIDs)
    o2f::AlgorithmSpec{[](o2f::InitContext &){

      //A common RNG provides randomly-distributed sleeping times
      auto rng= std::make_shared<std::mt19937_64>(std::random_device{}());
      auto distSleep= std::make_shared<std::uniform_int_distribution<>>(10, 100);
      auto distBoard= std::make_shared<std::uniform_int_distribution<>>(0, 8);
      auto distCath= std::make_shared<std::uniform_int_distribution<>>(0, 1);

      //Processing context in captured from return on InitCallback
      return [rng=rng,distSleep=distSleep,distBoard=distBoard,distCath=distCath](o2f::ProcessingContext &ctx){

        //Randomly sleeping
        std::this_thread::sleep_for(std::chrono::milliseconds((*distSleep)(*rng)));

        //A serializer instance is created
        o2::muon::mid::Serializer serializer;

        //Creating message
        for(int i=0; i<(*distBoard)(*rng); i++) {
          serializer.AddDigit(detElemID, (*distBoard)(*rng), 0, (*distCath)(*rng));
        }

        //Allocating a message in the right channel
        auto out = ctx.allocator().newChunk({"MID", "DIGITS", detElemID}, serializer.GetMessageSize());
        auto outI = reinterpret_cast<uint32_t *>(out.data);

        //Copying the message in the new pointer (DataChunk)
        auto msg = serializer.GetMessage();
        for(size_t msgI=0; msgI<serializer.GetMessageSize(); msgI++){
          outI[msgI] = std::move(msg[msgI]);
        }
      };
    }}
  };
}


o2f::DataProcessorSpec defineMIDRatesComputer(uint32_t detElemID){
  return {
    "MIDRatesComputer",
    o2f::Inputs{{"MIDBroadcaster", "MID", "DIGITS", detElemID, o2f::InputSpec::Lifetime::Timeframe}},
    o2f::Outputs{{"MID","RATES",detElemID,o2f::OutputSpec::Lifetime::Timeframe}},
    o2f::AlgorithmSpec{[](o2f::ProcessingContext &){
      auto algorithm = std::make_shared<o2::muon::mid::MIDRatesComputerAlgorithm>();
      algorithm.get()->Init("mapping.dat");
      return [ algo = algorithm ](o2f::ProcessingContext &){
        algo
      };
    }}
  };
}

o2f::WorkflowSpec MIDFilteringWorkflow()
{
  return o2f::WorkflowSpec();
}
