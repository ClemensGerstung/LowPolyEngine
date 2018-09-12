#pragma once

#include <map>

#include "ServiceBase.h"
#include "Uuid.h"
#include "Resource.h"

namespace lpe
{
  namespace utils
  {
    class IResourceManager : public ServiceBase
    {
    protected:
      std::map<Uuid, std::shared_ptr<Resource>> resources;

    public:
      IResourceManager() = default;
      virtual ~IResourceManager() override = default;

      virtual std::weak_ptr<Resource> Load(const char* fileName,
                                           const std::function<void(const uint8_t*,
                                                                    uint64_t)>& loaded = nullptr) = 0;
      virtual std::weak_ptr<Resource> Get(const Uuid& uuid) const = 0;
      virtual void Add(const Resource& resource) = 0;
    };


    /**
     * \brief Default ResourceManager for Windows/Linux PCs (using std::fstream)
     */
    class ResourceManager : public IResourceManager
    {
    public:
      ResourceManager() = default;
      ResourceManager(const ResourceManager& other);
      ResourceManager(ResourceManager&& other) noexcept;
      ResourceManager& operator=(const ResourceManager& other);
      ResourceManager& operator=(ResourceManager&& other) noexcept;
      ~ResourceManager() override = default;

      void Initialize() override;
      void Close() override;

      std::weak_ptr<Resource> Load(const char* fileName,
                                   const std::function<void(const uint8_t*,
                                                            uint64_t)>& loaded) override;
      std::weak_ptr<Resource> Get(const Uuid& uuid) const override;
      void Add(const Resource& resource) override;
    };
  }
}
