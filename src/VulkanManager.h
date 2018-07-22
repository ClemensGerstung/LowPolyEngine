#pragma once

#include "lpe.h"

namespace lpe
{
  namespace vulkan
  {
    class VulkanManager : public LpeObject
    {
      std::vector<const char*> validationLayers;
      std::vector<const char*> deviceExtensions;
      std::vector<const char*> instanceExtensions;

    public:
      static bool EnableValidationLayers;

      void Initialize() override;
      void Destroy() override;

      void SetValidationLayers(const std::vector<const char*>& layers);
      void SetValidationLayers(const char** layers, 
                               uint32_t layersCount);

      void SetDeviceExtensions(const std::vector<const char*>& extensionNames);
      void SetDeviceExtensions(const char** extensionNames,
                               uint32_t extensionCount);

      void SetInstanceExtensions(const std::vector<const char*>& extensionNames);
      void SetInstanceExtensions(const char** extensionNames,
                                 uint32_t extensionCount);
    };
  }
}