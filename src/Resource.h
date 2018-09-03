#pragma once
#include "Uuid.h"
#include <functional>
#include <vector>

namespace lpe
{
  namespace utils
  {
    class ResourceManager;

    class Resource
    {
    private:
      Uuid uuid;
      std::string physicalName;
      std::weak_ptr<ResourceManager> manager;
      std::vector<char> data;
    public:
      Resource(const std::shared_ptr<ResourceManager>& manager);
      Resource() = default;
      Resource(const Resource& resource);
      Resource(Resource&& resource);
      Resource& operator=(const Resource& resource);
      Resource& operator=(Resource&& resource);
      ~Resource();

      void Load(const char* fileName,
                const std::function<void(const char*,
                                         uint64_t)>& loaded = nullptr);
      void Load(const Uuid& uuid);
    };
  }
}
