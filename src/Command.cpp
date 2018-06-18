#include "Command.h"

lpe::vulkan::CommandPool::CommandPool(Instance* instance,
                                      PhysicalDevice* physicalDevice,
                                      Device* device,
                                      vk::CommandPoolCreateFlagBits flags,
                                      uint32_t queueIndex)
  : DeviceLevelObject(instance,
                      physicalDevice,
                      device),
    queueIndex(queueIndex)
{
  vk::CommandPoolCreateInfo createInfo = {
    flags,
    queueIndex
  };

  vk::Result result = VkDevice().createCommandPool(&createInfo,
                                                   nullptr,
                                                   &commandPool);
  helper::ThrowIfNotSuccess(result,
                            "could not create commandpool");
}

lpe::vulkan::CommandPool::~CommandPool()
{
  if (device)
  {
    VkDevice().destroyCommandPool(commandPool);
  }
}

lpe::vulkan::Command lpe::vulkan::CommandPool::AllocateSingle(vk::CommandBufferLevel level)
{
  return { instance.get(), physicalDevice.get(), device.get(), this, level };
}

lpe::vulkan::Commands lpe::vulkan::CommandPool::AllocateMany(uint32_t count,
                                                             vk::CommandBufferLevel level)
{
  return { instance.get(), physicalDevice.get(), device.get(), this, count, level };
}

lpe::vulkan::Command::Command(Instance* instance,
                              PhysicalDevice* physicalDevice,
                              Device* device,
                              CommandPool* pool,
                              vk::CommandBufferLevel level,
                              vk::CommandBuffer commandBuffer)
  : DeviceLevelObject(instance,
                      physicalDevice,
                      device),
    level(level),
    buffer(commandBuffer)
{
  this->pool.reset(pool);
}

lpe::vulkan::Command::Command(Instance* instance,
                              PhysicalDevice* physicalDevice,
                              Device* device,
                              CommandPool* pool,
                              vk::CommandBufferLevel level)
  : DeviceLevelObject(instance,
                      physicalDevice,
                      device),
    level(level)
{
  this->pool.reset(pool);

  vk::CommandBufferAllocateInfo allocateInfo =
  {
    *pool,
    level,
    1
  };

  vk::Result result = VkDevice().allocateCommandBuffers(&allocateInfo,
                                                        &buffer);
  helper::ThrowIfNotSuccess(result,
                            "could not allocate commandbuffer");
}

lpe::vulkan::Command::~Command()
{
  if (device)
  {
    VkDevice().freeCommandBuffers(*pool,
                                  1,
                                  &buffer);
  }
}

lpe::vulkan::Command::operator vk::CommandBuffer() const
{
  return buffer;
}

lpe::vulkan::Command::operator VkCommandBuffer() const
{
  return buffer;
}

lpe::vulkan::Commands::Commands(Instance* instance,
                                PhysicalDevice* physicalDevice,
                                Device* device,
                                CommandPool* pool,
                                uint32_t count,
                                vk::CommandBufferLevel level)
  : DeviceLevelObject(instance,
                      physicalDevice,
                      device),
    level(level)
{
  this->pool.reset(pool);

  vk::CommandBufferAllocateInfo allocateInfo =
  {
    *pool,
    level,
    count
  };

  buffers.resize(count);

  vk::Result result = VkDevice().allocateCommandBuffers(&allocateInfo,
                                                        buffers.data());
  helper::ThrowIfNotSuccess(result,
                            "could not allocate commandbuffer");
}

lpe::vulkan::Commands::~Commands()
{
  if (device)
  {
    VkDevice().freeCommandBuffers(*pool,
                                  static_cast<uint32_t>(buffers.size()),
                                  buffers.data());
  }
}
