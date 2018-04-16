//
// Created by Gabriele Gaetano Fronzé on 16/04/2018.
//

#include "DplOutputTest.h"

namespace o2
{
namespace workflows
{

o2f::DataProcessorSpec defineGenerator()
{
  return { "Generator",                                                             // Device name
           noInputs,                                                                // No inputs for a generator
           o2f::Outputs{ { "TST", "ToSink", 0, o2f::OutputSpec::Lifetime::Timeframe } }, // One simple output

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
                 ctx.allocator().newChunk({ "TST", "ToSink", 0, o2f::OutputSpec::Lifetime::Timeframe }, msgIndex + 1);

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

o2f::DataProcessorSpec defineSink()
{
  return { "Sink",                                                                   // Device name
           o2f::Inputs{ { "input", "TST", "ToSink", 0, o2f::InputSpec::Lifetime::Timeframe } }, // No inputs, for the moment
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

o2::framework::WorkflowSpec DplOutputTestWorkflow()
{
  auto lspec = o2f::WorkflowSpec();

  // A generator of data
  lspec.emplace_back(defineGenerator());
  lspec.emplace_back(defineSink());
  return std::move(lspec);
}

} // namespace workflows
} // namespace o2
