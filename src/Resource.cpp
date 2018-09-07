#include "Resource.h"
#include "ResourceManager.h"

#include <fstream>
#include <cassert>

lpe::utils::Resource::Resource(const std::shared_ptr<IResourceManager>& manager, const Uuid& uuid)
  : uuid(uuid)
{
  this->manager = manager;
  this->data = {};
}

lpe::utils::Resource::Resource()
{
  uuid = Uuid::GetNew();
  this->data = {};
}

lpe::utils::Resource::Resource(const Resource & resource)
{
  this->uuid = resource.uuid;
  this->data = resource.data;
  this->manager = resource.manager;
  this->physicalName = resource.physicalName;
}

lpe::utils::Resource::Resource(Resource && resource) noexcept
{
  this->uuid = std::move(resource.uuid);
  this->data = std::move(resource.data);
  this->manager = std::move(resource.manager);
  this->physicalName = std::move(resource.physicalName);
}

lpe::utils::Resource & lpe::utils::Resource::operator=(const Resource & resource)
{
  this->uuid = resource.uuid;
  this->data = resource.data;
  this->manager = resource.manager;
  this->physicalName = resource.physicalName;

  return *this;
}

lpe::utils::Resource & lpe::utils::Resource::operator=(Resource && resource) noexcept
{
  this->uuid = std::move(resource.uuid);
  this->data = std::move(resource.data);
  this->manager = std::move(resource.manager);
  this->physicalName = std::move(resource.physicalName);

  return *this;
}

lpe::utils::Resource::~Resource()
{
  this->manager.reset();
  this->data.clear();
}

void lpe::utils::Resource::Load(const char* fileName,
                                const std::function<void(const char*,
                                                         uint64_t)>& loaded)
{
  this->physicalName = fileName;
  std::ifstream ifs(fileName,
                    std::ios::binary | std::ios::ate);
  uint64_t size = ifs.tellg();
  data.resize(size);

  ifs.seekg(0,
            std::ios::beg);
  ifs.read(data.data(),
           size);
  ifs.close();

  if (loaded != nullptr)
  {
    loaded(data.data(),
           size);
  }
}

lpe::utils::Uuid lpe::utils::Resource::GetUuid() const
{
  return uuid;
}

uint64_t lpe::utils::Resource::GetData(const char* data) const
{
  data = this->data.data();

  return this->data.size();
}

