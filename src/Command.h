#pragma once

#include "DeviceLevelObject.h"

namespace lpe
{
  namespace vulkan
  {
    class Command;

    class CommandPool : public DeviceLevelObject
    {
    private:
      uint32_t queueIndex;
      vk::CommandPool commandPool;
    public:
      CommandPool() = default;
      CommandPool(const CommandPool& other);
      CommandPool(CommandPool&& other) noexcept;
      CommandPool operator=(const CommandPool& other);
      CommandPool operator=(CommandPool&& other) noexcept;
      
      CommandPool(Instance* instance,
                  PhysicalDevice* physicalDevice,
                  Device* device,
                  vk::CommandPoolCreateFlagBits falgs,
                  uint32_t queueIndex);

      ~CommandPool();

      Command AllocateSingle(vk::CommandBufferLevel level);
      std::vector<Command> AllocateMany(uint32_t count, vk::CommandBufferLevel level);

      operator vk::CommandPool() const;
      operator VkCommandPool() const;
    };

    class Command : DeviceLevelObject 
    {
    private:

    };
  }
}