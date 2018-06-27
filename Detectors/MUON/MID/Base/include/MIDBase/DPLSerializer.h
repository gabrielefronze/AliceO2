// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDBase/DPLSerializer.h
/// \brief  Templated boost serializer/deserializer for MID vectors in DPL
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   26 June 2018

#ifndef O2_MID_DPLSERIALIZER_H
#define O2_MID_DPLSERIALIZER_H

//#include <FairMQMessage.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "Framework/Output.h"
#include <ProcessingContext.h>

namespace o2
{
namespace mid
{
template <typename DataType>
void DPLSerialize(o2::framework::ProcessingContext& ctx, o2::framework::Output outSpec,
                  const std::vector<DataType>& dataVec)
{
  /// Serislizes a standard vector of template type DataType into a message
  std::ostringstream buffer;
  boost::archive::binary_oarchive outputArchive(buffer);
  outputArchive << dataVec;
  int size = buffer.str().length();
  auto msg = ctx.outputs().make<char>(outSpec, size);
  std::memcpy(&(msg[0]), buffer.str().c_str(), size);
}

template <typename DataType>
void DPLSerialize(o2::framework::ProcessingContext& ctx, o2::framework::Output outSpec,
                  const std::vector<DataType>& dataVec, const unsigned long nData)
{
  /// Serializes a standard vector of template type DataType into a message
  /// when the number of elements to serialize is smaller tham the
  /// vector size
  std::vector<DataType> copyVec(dataVec.begin(), dataVec.begin() + nData);
  DPLSerialize(ctx, std::move(outSpec), copyVec);
}

template <typename DataType>
void DPLDeserialize(char* msg, size_t msgSize, std::vector<DataType>& input)
{
  /// Deserializes the message into a standard vector of template type DataType
  input.clear();
  std::string msgStr(static_cast<char*>(msg), msgSize);
  std::istringstream buffer(msgStr);
  boost::archive::binary_iarchive inputArchive(buffer);
  inputArchive >> input;
}
} // namespace mid
} // namespace o2

#endif /* O2_MID_DPLSERIALIZER_H */
