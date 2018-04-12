//
// Created by Gabriele Gaetano Fronzé on 10/04/2018.
//

#include "FilteringChainDPL.h"
#include "Framework/DataProcessorSpec.h"
#include "MIDBase/FilteringSerializer.h"
#include "random"
#include "MIDRatesComputerAlgorithm.h"
#include "MIDMaskGeneratorAlgorithm.h"
#include "MIDFilterAlgorithm.h"

namespace o2f = o2::framework;

namespace o2
{
namespace workflows
{

o2f::Inputs noInputs{};
o2f::Outputs noOutputs{};

o2f::DataProcessorSpec defineFilteringBroadcaster()
{
  return { "Broadcaster", // Device name
           noInputs,      // No inputs, for the moment
           o2f::Outputs{ { "MID", "DIGITS", 0, o2f::OutputSpec::Lifetime::Timeframe },
                         { "MID", "DIGITS", 0,
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
                     distCath = distCath](o2f::ProcessingContext& ctx)
             {
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

o2::framework::DataProcessorSpec defineRatesComputer()
{
  return { "MIDRatesComputer", o2f::Inputs{ { "digits", "MID", "DIGITS", o2f::OutputSpec::Lifetime::Timeframe } },
           o2f::Outputs{ { "MID", "RATES", 0, o2f::OutputSpec::Lifetime::Timeframe } },

           o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             auto ratesComputerAlgorithm = std::make_shared<o2::mid::MIDRatesComputerAlgorithm>();
             (*ratesComputerAlgorithm).Init("binmap.dat");

             return [algo = ratesComputerAlgorithm](o2f::ProcessingContext& ctx) {
               auto digits = ctx.inputs().getByPos(1);
               std::vector<uint32_t> data;

               for (size_t iData = 1; iData < (digits.payload)[0]; iData++) {
                 data.emplace_back(digits.payload[iData]);
               }
               (*algo).Exec(data);

               // Allocating a message in the right channel
               for (auto const& itChannel : *(ctx.allocator().allowedChannels())) {
                 // Copying the message in the new pointer (DataChunk)
                 auto msg = (*(*algo).Output());

                 auto out = ctx.allocator().newChunk(itChannel.matcher, msg.size());
                 auto outI = reinterpret_cast<uint64_t*>(out.data);

                 for (size_t msgI = 0; msgI < msg.size(); msgI++) {
                   outI[msgI] = std::move(msg[msgI]);
                 }
               }
             };
           } } };
};

o2::framework::DataProcessorSpec defineMaskGenerator()
{
  return { "MIDMaskGenerator", o2f::Inputs{ { "rates", "MID", "RATES", o2f::OutputSpec::Lifetime::Timeframe } },
           o2f::Outputs{ { "MID", "MASK", 0, o2f::OutputSpec::Lifetime::QA } },

           o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             auto maskGeneratorAlgorithm = std::make_shared<o2::mid::MIDMaskGeneratorAlgorithm>();

             (*maskGeneratorAlgorithm).Init("binmap.dat");

             return [algo = maskGeneratorAlgorithm](o2f::ProcessingContext& ctx) {
               (*algo).Exec((uint64_t*)(ctx.inputs().getByPos(0).payload));

               // Allocating a message in the right channel
               for (auto const& itChannel : *(ctx.allocator().allowedChannels())) {
                 // Copying the message in the new pointer (DataChunk)
                 auto mask = (*(*algo).GetMask());

                 auto sum = mask.nDead + mask.nNoisy;
                 if (sum == 0)
                   return;
                 int msgSize = sizeof(mask.nDead) + sizeof(mask.nNoisy) + sum * sizeof(uint32_t);

                 auto out = ctx.allocator().newChunk(itChannel.matcher, msgSize);
                 auto header = reinterpret_cast<ushort_t*>(out.data);
                 header[0] = mask.nDead;
                 header[1] = mask.nNoisy;

                 auto payload = reinterpret_cast<uint32_t*>(&(header[2]));

                 int position = 0;

                 for (auto const& itDead : mask.deadStripsIDs) {
                   payload[position++] = itDead;
                 }
                 for (auto const& itNoisy : mask.noisyStripsIDs) {
                   payload[position++] = itNoisy;
                 }
               }
             };
           } } };
};

o2::framework::DataProcessorSpec defineFilter()
{
  return { "MIDFilter",
           o2f::Inputs{ { "digits", "MID", "DIGITS", o2f::OutputSpec::Lifetime::Timeframe },
                        { "mask", "MID", "MASK", o2f::OutputSpec::Lifetime::QA } },
           noOutputs, o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             auto filterAlgorithm = std::make_shared<o2::mid::MIDFilterAlgorithm>();
             (*filterAlgorithm).Init();

             return [algo = filterAlgorithm](o2f::ProcessingContext& ctx) {
               auto mask = ctx.inputs().getByPos(1);
               (*algo).ExecMaskLoading((ushort*)mask.header, (uint32_t*)mask.payload);

               auto digits = ctx.inputs().getByPos(0);
               std::vector<uint32_t> data;

               for (size_t iData = 1; iData < (digits.payload)[0]; iData++) {
                 data.emplace_back(digits.payload[iData]);
               }
               (*algo).ExecFilter(data);
             };
           } } };
};

o2f::WorkflowSpec MIDFilteringWorkflow()
{
  auto lspec = o2f::WorkflowSpec();
  lspec.emplace_back(defineFilter());
  lspec.emplace_back(defineRatesComputer());
  lspec.emplace_back(defineMaskGenerator());
  lspec.emplace_back(defineFilteringBroadcaster());
  return std::move(lspec);
}

} // namespace workflows
} // namespace o2