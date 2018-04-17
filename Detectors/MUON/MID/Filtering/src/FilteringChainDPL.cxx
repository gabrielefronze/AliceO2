//
// Created by Gabriele Gaetano Fronzé on 10/04/2018.
//

#include "FilteringChainDPL.h"
#include "Framework/DataProcessorSpec.h"
#include "Utils/Utils.h"
#include "MIDBase/FilteringSerializer.h"
#include "random"
#include "MIDRatesComputerAlgorithm.h"
#include "MIDMaskGeneratorAlgorithm.h"
#include "MIDFilterAlgorithm.h"
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
  return {
    "Generator",                                                   // Device name
    noInputs,                                                      // No inputs, for the moment
    o2f::Outputs{ { "MID", "GEN", 0, o2f::Lifetime::Timeframe } }, // Outputs are digits (aka DetElemIDs)

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

        auto out = ctx.outputs().newChunk({ "MID", "GEN", 0, o2f::Lifetime::Timeframe }, serializer.GetPayloadSize());
        auto outI = reinterpret_cast<uint32_t*>(out.data);

        // Copying the message in the new pointer (DataChunk)
        auto msg = serializer.GetPayload();
        for (size_t msgI = 0; msgI < serializer.GetPayloadSize(); msgI++) {
          outI[msgI] = std::move(msg[msgI]);
        }
      };
    } }
  };
}

o2::framework::DataProcessorSpec defineRatesComputer()
{
  o2f::InputSpec digits = { "digits", "MID", "DGTtoRtsComp", 0, o2f::Lifetime::Timeframe };

  return { "MIDRatesComputer", o2f::Inputs{ digits },
           o2f::Outputs{ { "MID", "RTStoMaskGen", 0, o2f::Lifetime::Timeframe } },

           o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             auto ratesComputerAlgorithm = std::make_shared<o2::mid::MIDRatesComputerAlgorithm>();
             (*ratesComputerAlgorithm).Init("~/alice_sw/test_device_data/binmapfile_MID.dat");

             return [algo = ratesComputerAlgorithm](o2f::ProcessingContext& ctx) {
               auto digits = ctx.inputs().get("digits");
               std::vector<uint32_t> data;

               for (size_t iData = 1; iData < (digits.payload)[0]; iData++) {
                 data.emplace_back(digits.payload[iData]);
               }
               (*algo).Exec(data);

               auto msg = (*(*algo).Output());

               auto out = ctx.outputs().newChunk({ "MID", "RTStoMaskGen", 0, o2f::Lifetime::Timeframe }, msg.size());
               auto outI = reinterpret_cast<uint64_t*>(out.data);

               for (size_t msgI = 0; msgI < msg.size(); msgI++) {
                 outI[msgI] = std::move(msg[msgI]);
               }
             };
           } } };
};

o2::framework::DataProcessorSpec defineMaskGenerator()
{
  o2f::InputSpec rates = { "rates", "MID", "RTStoMaskGen", 0, o2f::Lifetime::Timeframe };

  return { "MIDMaskGenerator", o2f::Inputs{ rates }, o2f::Outputs{ { "MID", "MSKtoFilter", 0, o2f::Lifetime::QA } },

           o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             auto maskGeneratorAlgorithm = std::make_shared<o2::mid::MIDMaskGeneratorAlgorithm>();

             (*maskGeneratorAlgorithm).Init("~/alice_sw/test_device_data/binmapfile_MID.dat");

             return [algo = maskGeneratorAlgorithm](o2f::ProcessingContext& ctx) {
               (*algo).Exec((uint64_t*)(ctx.inputs().get("rates").payload));

               // Copying the message in the new pointer (DataChunk)
               auto mask = (*(*algo).GetMask());

               auto sum = mask.nDead + mask.nNoisy;
               if (sum == 0)
                 return;
               int msgSize = sizeof(mask.nDead) + sizeof(mask.nNoisy) + sum * sizeof(uint32_t);

               auto out = ctx.outputs().newChunk({ "MID", "MSKtoFilter", 0, o2f::Lifetime::QA }, msgSize);
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
             };
           } } };
};

o2::framework::DataProcessorSpec defineFilter()
{
  o2f::InputSpec digits = { "digits", "MID", "DGTtoFilter", 0, o2f::Lifetime::Timeframe };
  o2f::InputSpec mask = { "mask", "MID", "MSKtoFilter", 0, o2f::Lifetime::QA };

  return { "MIDFilter", o2f::Inputs{ digits, mask }, noOutputs, o2f::AlgorithmSpec{ [](o2f::InitContext&) {
             auto filterAlgorithm = std::make_shared<o2::mid::MIDFilterAlgorithm>();
             (*filterAlgorithm).Init();

             return [algo = filterAlgorithm](o2f::ProcessingContext& ctx) {
               auto mask = ctx.inputs().get("mask");
               (*algo).ExecMaskLoading((ushort*)mask.payload, ((uint32_t*)(mask.payload)) + 25);

               auto digits = ctx.inputs().get("digits");
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

  std::function<size_t(o2f::DataRef)> f = [](o2f::DataRef data) -> size_t {
    return (size_t)(((uint32_t*)data.payload)[0]);
  };

  lspec.emplace_back(defineGenerator());
  lspec.emplace_back(defineBroadcaster("Broadcaster2",
                                       o2f::InputSpec{ "test", "MID", "GEN", 0, o2f::Lifetime::Timeframe },
                                       o2f::Outputs{ { "MID", "DGTtoFilter", 0, o2f::Lifetime::Timeframe },
                                                     { "MID", "DGTtoRtsComp", 0, o2f::Lifetime::Timeframe } }));
  lspec.emplace_back(defineFilter());
  lspec.emplace_back(defineRatesComputer());
  lspec.emplace_back(defineMaskGenerator());
  return std::move(lspec);
}

} // namespace workflows
} // namespace o2