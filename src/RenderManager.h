#pragma once

#include "ServiceBase.h"
#include "VkMemoryManagement.h"
#include "utils.h"

namespace lpe
{
  namespace render
  {
    class IRenderManager : public ServiceBase
    {
    public:
      ~IRenderManager() override = default;

      virtual void Draw() = 0;
    };

    class VulkanManager : public IRenderManager
    {
    private:
      vk::Instance instance;
      vk::PhysicalDevice physicalDevice;
      vk::Device device;
      vk::DeviceSize defaultSize;
      VkMemoryManagement dynamicMemory;
      VkStackAllocator stackAllocator;

      std::vector<const char*> instanceExtensions;
      std::vector<const char*> deviceExtensions;
      std::vector<const char*> layers;

      const char* applicationName;
      uint32_t applicationVersion;

      bool CheckInstanceExtensions();
      bool CheckInstanceLayers();
      void PickPhysicalDevice();

    public:
      VulkanManager();
      
      void Initialize() override;
      void Close() override;
      void Draw() override;

      VulkanManager& SetApplicationName(const char* applicationName);
      VulkanManager& SetApplicationVersion(uint16_t major, uint16_t minor, uint16_t patch);

      VulkanManager& SetDefaultMemoryChunkSize(vk::DeviceSize defaultSize);
      VulkanManager& AddInstanceLayer(const char* layerName);
      VulkanManager& AddInstanceExtension(const char* extensionName);
      VulkanManager& AddDeviceExtension(const char* extensionName);

      lpe::utils::SimplePointer<VkMemoryManagement> GetDeviceMemory() const;
      lpe::utils::SimplePointer<VkStackAllocator> GetHostMemory() const;
    };
  }
}
