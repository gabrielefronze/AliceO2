// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file DPLBroadcaster.cxx
/// \brief Implementation of generic DPL broadcaster, v0.1
///
/// \author Gabriele Gaetano Fronzé, gfronze@cern.ch

#include "Utils/Utils.h"
#include "Framework/DataProcessorSpec.h"
#include "Framework/DataProcessingHeader.h"
#include "Headers/DataHeader.h"

namespace o2f = o2::framework;

namespace o2
{
namespace workflows
{

// This is a possible implementation of a DPL compliant and generic broadcaster
o2f::DataProcessorSpec defineBroadcaster(std::string devName, o2f::InputSpec usrInput, o2f::Outputs usrOutputs,
                                         std::function<size_t(o2f::DataRef)> const& func)
{
  return { devName,      // Device name from user
           { usrInput }, // User defined input as a vector of one InputSpec
           usrOutputs,   // user defined outputs as a vector of OutputSpecs

           o2f::AlgorithmSpec{ [usrOutputs, func](o2f::InitContext&) {
             // Creating shared ptrs to useful parameters
             auto outputs_sharedptr = std::make_shared<o2f::Outputs>(std::move(usrOutputs));
             auto func_sharedptr = std::make_shared<std::function<size_t(o2f::DataRef)> const>(std::move(func));

             // Defining the ProcessCallback as returned object of InitCallback
             return [outputs_sharedptr, func_sharedptr](o2f::ProcessingContext& ctx) {
               // Getting original input message and getting his size using the provided function
               auto inputMsg = ctx.inputs().getByPos(0);

               auto msgSize = (*func_sharedptr)(inputMsg);

               // Iterating over the OutputSpecs to push the input message to all the output destinations
               for (const auto& itOutputs : (*outputs_sharedptr)) {
                 auto fwdMsg = ctx.allocator().newChunk(itOutputs, msgSize);
                 std::memcpy(fwdMsg.data, inputMsg.payload, msgSize);
               }
             };
           } } };
}

// This is a shortcut for messages with fixed user-defined size
o2f::DataProcessorSpec defineBroadcaster(std::string devName, o2f::InputSpec usrInput, o2f::Outputs usrOutputs,
                                         size_t fixMsgSize)
{
  auto func = [fixMsgSize](o2f::DataRef d) -> size_t { return fixMsgSize; };
  return defineBroadcaster(devName, usrInput, usrOutputs,func);
}
} // namespace workflows
} // namespace o2

// This is an implementation which retrieves the message size using the API
o2f::DataProcessorSpec defineBroadcaster(std::string devName, o2f::InputSpec usrInput, o2f::Outputs usrOutputs)
{
  return { devName,      // Device name from user
           { usrInput }, // User defined input as a vector of one InputSpec
           usrOutputs,   // user defined outputs as a vector of OutputSpecs

           o2f::AlgorithmSpec{ [usrOutputs](o2f::InitContext&) {
             // Creating shared ptrs to useful parameters
             auto outputs_sharedptr = std::make_shared<o2f::Outputs>(std::move(usrOutputs));

             // Defining the ProcessCallback as returned object of InitCallback
             return [outputs_sharedptr](o2f::ProcessingContext& ctx) {
               auto inputMsg = ctx.inputs().getByPos(0);
               auto msgSize = (o2::header::get<o2::header::DataHeader*>(inputMsg.header))->payloadSize;

               // Iterating over the OutputSpecs to push the input message to all the output destinations
               for (const auto& itOutputs : (*outputs_sharedptr)) {
                 auto fwdMsg = ctx.allocator().newChunk(itOutputs, msgSize);
                 std::memcpy(fwdMsg.data, inputMsg.payload, msgSize);
               }
             };
           } } };
}