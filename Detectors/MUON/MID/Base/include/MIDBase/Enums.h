//
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
///  @file   Enums
///  @author Gabriele G. Fronzé <gfronze at cern.ch>
///  @date   24 July 2017
///  @brief  Container for usefull enumerators
///

#ifndef O2_MID_ENUMS_H
#define O2_MID_ENUMS_H

namespace o2
{
namespace mid
{
typedef enum { kPhysics, kFET, kTriggered, kSize } digitType;

typedef enum { kShortMsg, kFailedSend, kOk } errMsg;
} // namespace mid
} // namespace o2

#endif // O2_MID_ENUMS_H
