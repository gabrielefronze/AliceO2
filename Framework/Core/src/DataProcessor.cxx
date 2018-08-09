// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
#include "Framework/DataProcessor.h"
#include "Framework/RootObjectContext.h"
#include "Framework/MessageContext.h"
#include "Framework/StringContext.h"
#include "Framework/RawBufferContext.h"
#include "Framework/TMessageSerializer.h"
#include "CommonUtils/BoostSerializer.h"
#include "Headers/DataHeader.h"
#include <fairmq/FairMQParts.h>
#include <fairmq/FairMQDevice.h>
#include <cstddef>

using namespace o2::framework;
using DataHeader = o2::header::DataHeader;

namespace o2
{
namespace framework
{

void DataProcessor::doSend(FairMQDevice &device, MessageContext &context) {
  for (auto &message : context) {
    //     monitoringService.send({ message.parts.Size(), "outputs/total" });
    assert(message.parts.Size() == 2);
    FairMQParts parts = std::move(message.parts);
    assert(message.parts.Size() == 0);
    assert(parts.Size() == 2);
    device.Send(parts, message.channel, 0);
    assert(parts.Size() == 2);
  }
}

void DataProcessor::doSend(FairMQDevice &device, RootObjectContext &context) {
  for (auto &messageRef : context) {
    assert(messageRef.payload.get());
    FairMQParts parts;
    FairMQMessagePtr payload(device.NewMessage());
    auto a = messageRef.payload.get();
    device.Serialize<TMessageSerializer>(*payload, a);
    const DataHeader* cdh = o2::header::get<DataHeader*>(messageRef.header->GetData());
    // sigh... See if we can avoid having it const by not
    // exposing it to the user in the first place.
    DataHeader *dh = const_cast<DataHeader *>(cdh);
    dh->payloadSize = payload->GetSize();
    parts.AddPart(std::move(messageRef.header));
    parts.AddPart(std::move(payload));
    device.Send(parts, messageRef.channel, 0);
  }
}

void DataProcessor::doSend(FairMQDevice& device, StringContext& context)
{
  for (auto& messageRef : context) {
    FairMQParts parts;
    FairMQMessagePtr payload(device.NewMessage());
    auto a = messageRef.payload.get();
    // Rebuild the message using the string as input. For now it involves a copy.
    payload->Rebuild(reinterpret_cast<void*>(const_cast<char*>(strdup(a->data()))), a->size(), nullptr, nullptr);
    const DataHeader* cdh = o2::header::get<DataHeader*>(messageRef.header->GetData());
    // sigh... See if we can avoid having it const by not
    // exposing it to the user in the first place.
    DataHeader* dh = const_cast<DataHeader*>(cdh);
    dh->payloadSize = payload->GetSize();
    parts.AddPart(std::move(messageRef.header));
    parts.AddPart(std::move(payload));
    device.Send(parts, messageRef.channel, 0);
  }
}

void DataProcessor::doSend(FairMQDevice& device, RawBufferContext& context)
{

  LOG(INFO) << "Sending...";
  for (auto& messageRef : context) {
    FairMQParts parts;
    FairMQMessagePtr payload(device.NewMessage());
    auto buffer = messageRef.serializeMsg().str();
    // Rebuild the message using the serialized ostringstream as input. For now it involves a copy.
    size_t size = buffer.length();
    payload->Rebuild(size);
    std::memcpy(payload->GetData(), buffer.c_str(), size);
    const DataHeader* cdh = o2::header::get<DataHeader*>(messageRef.header->GetData());
    // sigh... See if we can avoid having it const by not
    // exposing it to the user in the first place.
    DataHeader* dh = const_cast<DataHeader*>(cdh);
    dh->payloadSize = size;
    LOG(INFO) << "length set to " << dh->payloadSize;
    parts.AddPart(std::move(messageRef.header));
    parts.AddPart(std::move(payload));
    device.Send(parts, messageRef.channel, 0);
    LOG(INFO) << "DONE!";
  }
  LOG(INFO) << "DONE!";
}

} // namespace framework
} // namespace o2
