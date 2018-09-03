#include "Resource.h"
#include "ResourceManager.h"

#include <fstream>
#include <cassert>

lpe::utils::Resource::Resource(const std::shared_ptr<ResourceManager>& manager)
  : uuid()
{
  this->manager = manager;
  this->physicalName = nullptr;
  this->data = {};
}

void lpe::utils::Resource::Load(const char* fileName,
                                const std::function<void(const char*,
                                                         uint64_t)>& loaded)
{
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

void lpe::utils::Resource::Load(const Uuid& uuid)
{
  auto rm = this->manager.lock();
  assert(rm);


}
