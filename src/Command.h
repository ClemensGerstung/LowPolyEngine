#pragma once

#include "DeviceLevelObject.h"

namespace lpe
{
  namespace vulkan
  {
    class Command;
    class Commands;

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
                  vk::CommandPoolCreateFlagBits flags,
                  uint32_t queueIndex);

      virtual ~CommandPool();

      Command AllocateSingle(vk::CommandBufferLevel level);
      Commands AllocateMany(uint32_t count,
                            vk::CommandBufferLevel level);

      operator vk::CommandPool() const;
      operator VkCommandPool() const;
    };

    class Command : DeviceLevelObject
    {
    private:
      std::unique_ptr<CommandPool> pool;
      vk::CommandBufferLevel level;
      vk::CommandBuffer buffer;

    public:
      Command() = default;
      Command(const Command& other);
      Command(Command&& other) noexcept;
      Command& operator=(const Command& other);
      Command& operator=(Command&& other) noexcept;

      Command(Instance* instance,
              PhysicalDevice* physicalDevice,
              Device* device,
              CommandPool* pool,
              vk::CommandBufferLevel level,
              vk::CommandBuffer commandBuffer);

      Command(Instance* instance,
              PhysicalDevice* physicalDevice,
              Device* device,
              CommandPool* pool,
              vk::CommandBufferLevel level);

      ~Command();

      operator vk::CommandBuffer() const;
      operator VkCommandBuffer() const;
    };

    class Commands : DeviceLevelObject
    {
    private:
      std::unique_ptr<CommandPool> pool;
      vk::CommandBufferLevel level;
      std::vector<vk::CommandBuffer> buffers;
    public:
      Commands() = default;
      Commands(const Commands& other);
      Commands(Commands&& other) noexcept;
      Commands& operator=(const Commands& other);
      Commands& operator=(Commands&& other) noexcept;

      Commands(Instance* instance,
               PhysicalDevice* physicalDevice,
               Device* device,
               CommandPool* pool,
               uint32_t count,
               vk::CommandBufferLevel level);

      ~Commands();
    };
  }
}
