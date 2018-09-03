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

