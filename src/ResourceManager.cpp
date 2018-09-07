#include "ResourceManager.h"
#include <cassert>

lpe::utils::ResourceManager::ResourceManager(const ResourceManager& other)
{
}

lpe::utils::ResourceManager::ResourceManager(ResourceManager&& other) noexcept
{
}

lpe::utils::ResourceManager& lpe::utils::ResourceManager::operator=(const ResourceManager& other)
{
  return *this;
}

lpe::utils::ResourceManager& lpe::utils::ResourceManager::operator=(ResourceManager&& other) noexcept
{
  return *this;
}

void lpe::utils::ResourceManager::Initialize()
{
}

void lpe::utils::ResourceManager::Close()
{
}

std::weak_ptr<lpe::utils::Resource> lpe::utils::ResourceManager::Load(const char* fileName,
                                                                      const std::function<void(const char*,
                                                                                               uint64_t)>& loaded)
{
  Uuid uuid = Uuid::GetNew();
  auto ptr = std::make_shared<Resource>(std::make_shared<ResourceManager>(*this),
                                        uuid);

  ptr->Load(fileName,
            loaded);

  resources.insert(std::make_pair(uuid, ptr));
  
  return resources[uuid];
}

std::weak_ptr<lpe::utils::Resource> lpe::utils::ResourceManager::Get(const Uuid& uuid) const
{
  auto result = resources.find(uuid);

  assert(result != std::end(resources));

  return result->second;
}

void lpe::utils::ResourceManager::Add(const Resource& resource)
{
  Uuid uuid = resource.GetUuid();
  auto ptr = std::make_shared<Resource>(resource);

  resources.emplace(uuid, ptr);
}
