// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   MID/Filtering/DPL/MIDFilteringDPL.h
/// \brief  A DPL workflow specification to perform noisy channels filtering and dead channels detection for MID
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   26 June 2018

#ifndef O2_MID_MIDFILTERINGDPL_H
#define O2_MID_MIDFILTERINGDPL_H

#include <Framework/runDataProcessing.h>

o2::framework::WorkflowSpec MIDFilteringDPLWorkflow();

#endif //O2_MID_MIDFILTERINGDPL_H
