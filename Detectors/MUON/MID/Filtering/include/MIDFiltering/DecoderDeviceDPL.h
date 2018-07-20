// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MIDFiltering/DecoderDeviceDPL.h
/// \brief  Implementation of CRU decoder device for the MID in DPL
/// \author Gabriele Gaetano Fronzé <gfronze at cern.ch>
/// \date   20 July 2018

#ifndef O2_MID_DECODERDEVICEDPL_H
#define O2_MID_DECODERDEVICEDPL_H

#include "MIDFiltering/Decoder.h"
#include "Framework/Output.h"
#include "Framework/Task.h"
#include <vector>
#include <string>

namespace o2
{
namespace mid
{
class DecoderDeviceDPL
{
 public:
  DecoderDeviceDPL(std::string inputBinding,
                   std::string outputBinding) : mDecoder()
  {
    mInputBinding = inputBinding;
    mOutputBinding = outputBinding;
  };

  ~DecoderDeviceDPL(){};

  void init(o2::framework::InitContext& ic);

  void run(o2::framework::ProcessingContext& pc);

 private:
  Decoder mDecoder;
  std::string mInputBinding;
  std::string mOutputBinding;
};
} // namespace mid

} // namespace o2

#endif //O2_MID_DECODERDEVICEDPL_H
