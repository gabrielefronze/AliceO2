// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file DPLGatherer.cxx
/// \brief Implementation of generic DPL gatherer, v0.1
///
/// \author Gabriele Gaetano Fronzé, gfronze@cern.ch

#include "Utils/Utils.h"
#include "Framework/DataProcessorSpec.h"
#include <vector>

namespace o2f = o2::framework;

namespace o2
{
namespace workflows
{

// This is a possible implementation of a DPL compliant and generic gatherer
o2f::DataProcessorSpec defineGatherer(std::string devName, o2f::Inputs usrInputs, o2f::OutputSpec usrOutput,
                                      std::function<void(OutputBuffer,const o2f::DataRef)> const& mergerFunc)
{
  return { devName,       // Device name from user
           usrInputs,     // User defined input as a vector of one InputSpec
           { usrOutput }, // user defined outputs as a vector of OutputSpecs

           o2f::AlgorithmSpec{ [usrOutput, mergerFunc](o2f::InitContext&) {
             // Creating shared ptrs to useful parameters
             auto mergerFunc_sharedptr = std::make_shared<std::function<void(OutputBuffer,o2f::DataRef)> const>(mergerFunc);

             // Defining the ProcessCallback as returned object of InitCallback
             return [usrOutput, mergerFunc_sharedptr](o2f::ProcessingContext& ctx) {

               OutputBuffer outputBuffer;

               // Iterating over the InputSpecs to aggregate msgs from the connected devices
               for (const auto& itInputs : ctx.inputs() ) {
                 (*mergerFunc_sharedptr)(outputBuffer,itInputs);
               }

               ctx.allocator().adoptChunk(usrOutput,&outputBuffer[0],outputBuffer.size(),&header::Stack::freefn,nullptr);
             };
           } } };
}

// This is a possible implementation of a DPL compliant and generic gatherer whit trivial messages concatenation
o2f::DataProcessorSpec defineGatherer(std::string devName, o2f::Inputs usrInputs, o2f::OutputSpec usrOutput)
{
  return { devName,       // Device name from user
           usrInputs,     // User defined input as a vector of one InputSpec
           { usrOutput }, // user defined outputs as a vector of OutputSpecs

           o2f::AlgorithmSpec{ [usrOutput](o2f::InitContext&) {
             // Defining the ProcessCallback as returned object of InitCallback
             return [usrOutput](o2f::ProcessingContext& ctx) {

               OutputBuffer outputBuffer;

               // Iterating over the InputSpecs to aggregate msgs from the connected devices
               for (const auto& itInput : ctx.inputs() ) {
                 // Getting message size through the API
                 auto msgSize = (o2::header::get<o2::header::DataHeader*>(itInput.header))->payloadSize;

                 // Resizing the buffer
                 outputBuffer.resize(outputBuffer.size()+msgSize);

                 // Appending received message to outputBuffer
                 std::copy(itInput.payload[0],itInput.payload[msgSize-1],std::back_inserter(outputBuffer));
               }

               ctx.allocator().adoptChunk(usrOutput,&outputBuffer[0],outputBuffer.size(),&header::Stack::freefn,nullptr);
             };
           } } };
}

} // namespace workflows
} // namespace o2