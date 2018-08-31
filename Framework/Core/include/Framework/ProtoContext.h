// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file   ProtoContext.h
/// \brief  **********
/// \author Gabriele G. Fronzé <gfronze at cern.ch>
/// \date   31/07/2018

#ifndef FRAMEWORK_PROTOCONTEXT_H
#define FRAMEWORK_PROTOCONTEXT_H

#include <cstddef>
#include <cassert>

namespace o2
{
namespace framework
{

using context_id_type = size_t;

class ProtoContext
{
 public:
  virtual context_id_type getID() = 0;
};

template<typename dummyT = void> 
constexpr context_id_type ContextIDGenerator(const char *str) {
    static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4,"context_id_type not compatible (size must be 4 or 8)");
    size_t h = 0;
    if (sizeof(size_t) == 8) {
        h = 1125899906842597L; // prime
    } else {
        h = 4294967291L;
    }
    int i = 0;
    while (str[i] != 0) {
        h = 31 * h + str[i++];
    }

    return h;
}
}
}

#endif // FRAMEWORK_PROTOCONTEXT_H