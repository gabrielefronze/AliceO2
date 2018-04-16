//
// Created by Gabriele Gaetano Fronzé on 15/04/2018.
//

#include <fstream>
#include "DPLBroadcasterGatherer.h"
#include "../include/Utils/Utils.h"
#include "Framework/DataProcessorSpec.h"
#include "random"
#include "FairMQLogger.h"

namespace o2f = o2::framework;

namespace o2
{
namespace workflows
{

o2f::Inputs noInputs{};
o2f::Outputs noOutputs{};

o2f::DataProcessorSpec defineGenerator()
{
  return { "Generator",                                                           // Device name
           noInputs,                                                              // No inputs for a generator
           o2f::Outputs{ { "A", "B", 0, o2f::OutputSpec::Lifetime::Timeframe } }, // One simple output

           o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             int msgCounter = 0;
             auto msgCounter_shptr = std::make_shared<int>(msgCounter);

             LOG(INFO) << ">>>>>>>>>>>>>> Generator initialised\n";

             // Processing context in captured from return on InitCallback
             return [msgCounter_shptr](o2f::ProcessingContext& ctx) {
               int msgIndex = (*msgCounter_shptr)++;
               LOG(INFO) << ">>> MSG:" << msgIndex << "\n";
               std::this_thread::sleep_for(std::chrono::milliseconds(1000));

               LOG(INFO) << ">>> Preparing MSG:" << msgIndex << "\n";

               auto outputMsg =
                 ctx.allocator().newChunk({ "A", "GEN", 0, o2f::OutputSpec::Lifetime::Timeframe }, msgIndex + 1);

               LOG(INFO) << ">>> Preparing1 MSG:" << msgIndex << "\n";

               auto payload = reinterpret_cast<uint32_t*>(outputMsg.data);

               payload[0] = msgIndex;

               LOG(INFO) << ">>> Preparing2 MSG:" << msgIndex << "\n";

               for (int k = 0; k < msgIndex; ++k) {
                 payload[k + 1] = (uint32_t)32;
                 LOG(INFO) << ">>>>\t" << payload[k + 1] << "\n";
               }
             };
           } } };
}

o2f::DataProcessorSpec definePipeline(std::string devName, o2f::InputSpec usrInput, o2f::OutputSpec usrOutput)
{
  return { devName,                 // Device name
           o2f::Inputs{ usrInput }, // No inputs, for the moment
           o2f::Outputs{ usrOutput }, o2f::AlgorithmSpec{ [usrOutput](o2f::InitContext&) {
             auto output_sharedptr = std::make_shared<o2f::OutputSpec>(std::move(usrOutput));

             // Processing context in captured from return on InitCallback
             return [output_sharedptr](o2f::ProcessingContext& ctx) {
               auto inputMsg = ctx.inputs().get("d");
               auto msgSize = (o2::header::get<o2::header::DataHeader*>(inputMsg.header))->payloadSize;

               auto fwdMsg = ctx.allocator().newChunk((*output_sharedptr), msgSize);
               std::memcpy(fwdMsg.data, inputMsg.payload, msgSize);
             };
           } } };
}

o2f::DataProcessorSpec defineSink()
{
  return { "Sink",                                                                   // Device name
           o2f::Inputs{ { "d", "D", "E", 0, o2f::InputSpec::Lifetime::Timeframe } }, // No inputs, for the moment
           noOutputs,

           o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             // Processing context in captured from return on InitCallback
             return [](o2f::ProcessingContext& ctx) {
               auto inputMsg = ctx.inputs().get("d");
               auto payload = reinterpret_cast<const uint32_t*>(inputMsg.payload);

               LOG(INFO) << "Received message containing" << payload[0] << "elements\n";
               for (int j = 0; j < payload[0]; ++j) {
                 LOG(INFO) << payload[j] << "\t";
               }
               LOG(INFO) << "\n";
             };
           } } };
}

o2::framework::WorkflowSpec DPLBroadcasterGathererWorkflow()
{
  auto lspec = o2f::WorkflowSpec();

  // A generator of data
  lspec.emplace_back(defineGenerator());

  // A two-way broadcaster
  lspec.emplace_back(defineBroadcaster("Broadcaster",
                                       o2f::InputSpec{ "c", "A", "B", 0, o2f::InputSpec::Lifetime::Timeframe },
                                       o2f::Outputs{ { "A", "BCAST0", 0, o2f::OutputSpec::Lifetime::Timeframe },
                                                     { "A", "BCAST1", 0, o2f::OutputSpec::Lifetime::Timeframe } },
                                       [](o2f::DataRef data) { return (size_t)data.payload[0]; }));

  // Two pipeline devices
  lspec.emplace_back(definePipeline("pip0",
                                    o2f::InputSpec{ "bc", "A", "BCAST0", 0, o2f::InputSpec::Lifetime::Timeframe },
                                    o2f::OutputSpec{ "A", "PIP0", 0, o2f::OutputSpec::Lifetime::Timeframe }));
  lspec.emplace_back(definePipeline("pip1",
                                    o2f::InputSpec{ "bc", "A", "BCAST1", 0, o2f::InputSpec::Lifetime::Timeframe },
                                    o2f::OutputSpec{ "A", "PIP1", 0, o2f::OutputSpec::Lifetime::Timeframe }));

  // A gatherer
  lspec.emplace_back(defineGatherer("Gatherer",
                                    o2f::Inputs{ { "input1", "A", "PIP0", 0, o2f::InputSpec::Lifetime::Timeframe },
                                                 { "input2", "A", "PIP1", 0, o2f::InputSpec::Lifetime::Timeframe } },
                                    o2f::OutputSpec{ "D", "E", 0, o2f::OutputSpec::Lifetime::Timeframe },
                                    [](OutputBuffer outputBuffer, const o2f::DataRef data) {
                                      auto payload = reinterpret_cast<const uint32_t*>(data.payload);
                                      outputBuffer[0] += payload[0];
                                      std::copy(&(payload[1]), &(payload[payload[0]]),
                                                std::back_inserter(outputBuffer));
                                    }));

  // A sink which dumps messages
  lspec.emplace_back(defineSink());
  return std::move(lspec);
}

} // namespace workflows
} // namespace o2