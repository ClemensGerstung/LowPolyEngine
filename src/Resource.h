#pragma once
#include <functional>
#include <vector>

#include "Uuid.h"

namespace lpe
{
  namespace utils
  {
    class IResourceManager;

    class Resource
    {
    private:
      Uuid uuid;
      std::string physicalName;
      std::weak_ptr<IResourceManager> manager;
      std::vector<char> data;
    public:
      Resource(const std::shared_ptr<IResourceManager>& manager, const Uuid& uuid);
      Resource();
      Resource(const Resource& resource);
      Resource(Resource&& resource);
      Resource& operator=(const Resource& resource);
      Resource& operator=(Resource&& resource);
      ~Resource();

      void Load(const char* fileName,
                const std::function<void(const char*,
                                         uint64_t)>& loaded = nullptr);

      Uuid GetUuid() const;
    };
  }
}
