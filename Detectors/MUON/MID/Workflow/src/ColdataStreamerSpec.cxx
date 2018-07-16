// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   ColdataStreamerSpec.cxx
/// \brief  Data processor spec for generic MID ColumnData streamer
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   11 July 2018

#include <chrono>
#include <thread>
#include "Framework/DataProcessorSpec.h"
#include "Framework/AlgorithmSpec.h"
#include "DataFormatsMID/ColumnData.h"
#include "Framework/DPLBoostSerializer.h"
#include "ColdataStreamerSpec.h"

namespace of = o2::framework;

namespace o2
{
namespace mid
{
framework::DataProcessorSpec getColdataStreamerSpec()
{
  std::string outputBinding = "mid_coldata";
  auto outputChannel = of::Outputs{ of::OutputSpec{ "MID", "COLDATA" } };
  outputChannel.back().binding.value = outputBinding;

  auto count_shptr = std::make_shared<size_t>(0);

  return of::DataProcessorSpec{
    "ColdataStreamer",
    of::Inputs{},
    outputChannel,
    of::AlgorithmSpec{
      [outputBinding, count_shptr](of::ProcessingContext& pc) {
        std::this_thread::sleep_for(std::chrono::microseconds(20));

        std::vector<ColumnData> dataVect;
        (*(count_shptr.get()))++;
        LOG(INFO) << "Creating " << (*(count_shptr.get())) << " ColumnDatas.";
        for (int j = 0; j < *(count_shptr.get()); ++j) {
          o2::mid::ColumnData columnData;
          for (auto& itPattern : columnData.patterns) {
            columnData.deId = 17;
            columnData.columnId = 2;
            itPattern = (1 << 16) - 1;
          }
          dataVect.emplace_back(columnData);
        }
        o2::framework::DPLBoostSerialize(pc, outputBinding, dataVect);
      } }
  };
};
} // namespace mid
} // namespace o2