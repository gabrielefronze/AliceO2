// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   DecoderDeviceDPL.cxx
/// \brief  Implementation of CRU decoder device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#include "MIDFiltering/DecoderDeviceDPL.h"
#include "Framework/DPLBoostSerializer.h"

namespace o2
{
namespace mid
{

void DecoderDeviceDPL::init(o2::framework::InitContext& ic)
{
  mDecoder.resetOffset();
}

void DecoderDeviceDPL::run(o2::framework::ProcessingContext& pc)
{
  static_assert(std::is_same<std::uint8_t, char>::value ||
                  std::is_same<std::uint8_t, unsigned char>::value,
                "This library requires std::uint8_t to be implemented as char or unsigned char.");

  auto payload = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(pc.inputs().get(mInputBinding).payload));
  auto outputVect = mDecoder.decodeFEEData(payload);
  framework::DPLBoostSerialize(pc, mOutputBinding, outputVect);
}
} // namespace mid
} // namespace o2
