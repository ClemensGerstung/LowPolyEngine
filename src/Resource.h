#pragma once
#include "Uuid.h"
#include <functional>

namespace lpe
{
  namespace utils
  {
    class ResourceManager;

    enum class ResourceType
    {
      Mesh,
      Texture,
      Shader,
      SPIRV,
    };

    class Resource
    {
    private:
      Uuid uuid;
      std::string physicalName;
      ResourceType type;
      std::weak_ptr<ResourceManager> manager;
    public:
      Resource(const ResourceManager* manager);
      Resource() = default;
      Resource(const Resource& resource);
      Resource(Resource&& resource);
      Resource& operator=(const Resource& resource);
      Resource& operator=(Resource&& resource);
      ~Resource();

      void Load(const char* fileName, ResourceType type, std::function<void(const std::fstream&)> loaded);
      void Load(const Uuid& uuid);
    };
  }
}
