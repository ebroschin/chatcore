#pragma once

#include <unordered_map>

#include "resource.h"

namespace claw::utility {

template<typename TResource>
class ResourceStore {
public:
  template<typename... TResourceArgs>
  TResource* Create(TResourceArgs&&... args);

  void Destroy(const TResource* resource);

private:
  std::unordered_map<typename TResource::ValueType*, TResource> store_;
};

template<typename TResource>
template<typename... TResourceArgs>
TResource* ResourceStore<TResource>::Create(TResourceArgs&&... args) {
  TResource resource{std::forward<TResourceArgs>(args)...};
  auto key = resource.Get();
  auto [iterator, success] = store_.emplace(key, std::move(resource));
  return success? &iterator->second : nullptr;
}

template<typename TResource>
void ResourceStore<TResource>::Destroy(const TResource* resource) {
  if (!resource) return;

  auto iterator = store_.find(resource->Get());
  if (iterator == store_.end()) return;

  store_.erase(iterator);
}

}