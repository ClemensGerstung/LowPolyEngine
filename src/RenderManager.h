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
      virtual ~IRenderManager() = default;

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

    public:
      VulkanManager();
      
      void Initialize() override;
      void Close() override;
      void Draw() override;

      VulkanManager& SetDefaultMemoryChunkSize(vk::DeviceSize defaultSize);
      VulkanManager& AddLayer(const char* layerName);
      VulkanManager& AddExtension(const char* extensionName);

      lpe::utils::SimplePointer<VkMemoryManagement> GetDeviceMemory() const;
      lpe::utils::SimplePointer<VkStackAllocator> GetHostMemory() const;
    };
  }
}
