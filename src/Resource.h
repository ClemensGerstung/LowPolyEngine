#pragma once
#include <functional>
#include <vector>
#include <memory>

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
      std::vector<uint8_t> data;
    public:
      Resource(const std::shared_ptr<IResourceManager>& manager, const Uuid& uuid);
      Resource();
      Resource(const Resource& resource);
      Resource(Resource&& resource) noexcept;
      Resource& operator=(const Resource& resource);
      Resource& operator=(Resource&& resource) noexcept;
      ~Resource();

      void Load(const char* fileName,
                const std::function<void(const uint8_t*,
                                         uint64_t)>& loaded = nullptr);

      Uuid GetUuid() const;
      uint64_t GetData(const uint8_t** data) const;
      uint64_t GetSize() const;
    };
  }
}
