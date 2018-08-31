// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef FRAMEWORK_CONTEXTREGISTRY_H
#define FRAMEWORK_CONTEXTREGISTRY_H

#include <vector>
#include <unordered_map>
#include <cassert>
#include "Framework/ProtoContext.h"

namespace o2
{
namespace framework
{
/// Decouples getting the various contextes from the actual type
/// of context, so that the DataAllocator does not need to know
/// about the various serialization methods. Since there is only
/// a few context types we support, this can be done in an ad hoc
/// manner making sure each overload of ContextRegistry<T>::get()
/// uses a different entry in ContextRegistry::contextes;
///
/// Right now we use:
///
/// MessageContext 0
/// ROOTObjectContext 1
/// StringContext 2
/// RawContext 3
class ContextRegistry
{
 public:
  ContextRegistry(std::vector<ProtoContext*> contextes)
  {
    for(auto &itCtx : contextes){
      auto ID = itCtx->getID();
      mContextes[ID] = itCtx;
    }
  }

  /// Helper to get the context from the registry.
template <class ContextT>
inline ContextT* get()
{
  auto ID = ContextT::mContextID;
  auto got = mContextes.find(ID);
  assert(got != mContextes.end());
  return reinterpret_cast<ContextT*>(got->second);
}

/// Helper to set the context from the registry.
template <class ContextT>
inline void set(ContextT* context)
{
  if (mContextes.find(context->getID()) == mContextes.end()) {
    mContextes.insert(std::make_pair<context_id_type,ProtoContext*>(context->getID(),context));
  }
}

 private:
  std::unordered_map<context_id_type,ProtoContext*> mContextes;
};

} // namespace framework
} // namespace o2
#endif // FRAMEWORK_CONTEXTREGISTRY_H
