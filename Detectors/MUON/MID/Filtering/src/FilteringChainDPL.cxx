//
// Created by Gabriele Gaetano Fronzé on 10/04/2018.
//

#include "FilteringChainDPL.h"
#include "Framework/DataProcessorSpec.h"
#include "MIDBase/FilteringSerializer.h"
#include "random"

namespace o2f = o2::framework;

namespace o2
{
namespace workflows
{

o2f::Inputs noInputs{};
o2f::Outputs noOutputs{};

const uint64_t toFilter = 1;
const uint64_t toMaskGenerator = 2;

o2f::DataProcessorSpec defineFilteringBroadcaster()
{
  return { "Broadcaster", // Device name
           noInputs,      // No inputs, for the moment
           o2f::Outputs{ { "MID", "DIGITS", toFilter, o2f::OutputSpec::Lifetime::Timeframe },
                         { "MID", "DIGITS", toMaskGenerator,
                           o2f::OutputSpec::Lifetime::Timeframe } }, // Outputs are digits (aka DetElemIDs)

           o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             // A common RNG provides randomly-distributed sleeping times
             auto rng = std::make_shared<std::mt19937_64>(std::random_device{}());

             auto distSleep = std::make_shared<std::uniform_int_distribution<>>(10, 100);

             auto distRPC = std::make_shared<std::uniform_int_distribution<>>(0, 71);
             auto distColumn = std::make_shared<std::uniform_int_distribution<>>(0, 8);
             auto distCath = std::make_shared<std::uniform_int_distribution<>>(0, 1);

             // Processing context in captured from return on InitCallback
             return [rng = rng, distSleep = distSleep, distRPC = distRPC, distColumn = distColumn,
                     distCath = distCath](o2f::ProcessingContext& ctx) {
               // Randomly sleeping
               std::this_thread::sleep_for(std::chrono::milliseconds((*distSleep)(*rng)));

               // A serializer instance is created
               o2::mid::FilteringSerializer serializer;

               // Creating message
               for (int i = 0; i < (*distColumn)(*rng); i++) {
                 serializer.AddDigit((*distRPC)(*rng), (*distColumn)(*rng), 0, (*distCath)(*rng));
               }

               // Allocating a message in the right channel
               for (auto const& itChannel : *(ctx.allocator().allowedChannels())) {

                 auto out = ctx.allocator().newChunk(itChannel.matcher, serializer.GetMessageSize());
                 auto outI = reinterpret_cast<uint32_t*>(out.data);

                 // Copying the message in the new pointer (DataChunk)
                 auto msg = serializer.GetMessage();
                 for (size_t msgI = 0; msgI < serializer.GetMessageSize(); msgI++) {
                   outI[msgI] = std::move(msg[msgI]);
                 }
               }
             };
           } } };
}

o2f::WorkflowSpec MIDFilteringWorkflow() {
  auto lspec = o2f::WorkflowSpec();
  lspec.emplace_back(defineFilteringBroadcaster());
  return std::move(lspec);
}

} // namespace workflow
} // namespace o2