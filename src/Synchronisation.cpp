#include "Synchronisation.h"

lpe::vulkan::Semaphore::Semaphore(const Semaphore& other)
{
  semaphore = other.semaphore;
}

lpe::vulkan::Semaphore::Semaphore(Semaphore&& other) noexcept
{
  semaphore = other.semaphore;
}

lpe::vulkan::Semaphore& lpe::vulkan::Semaphore::operator=(const Semaphore& other)
{
  semaphore = other.semaphore;
  return *this;
}

lpe::vulkan::Semaphore& lpe::vulkan::Semaphore::operator=(Semaphore&& other) noexcept
{
  semaphore = other.semaphore;
  return *this;
}

lpe::vulkan::Semaphore::Semaphore(Instance* instance,
                                  PhysicalDevice* physicalDevice,
                                  Device* device)
  : DeviceLevelObject(instance,
                      physicalDevice,
                      device)
{
  vk::SemaphoreCreateInfo createInfo = {};

  vk::Result result = VkDevice().createSemaphore(&createInfo,
                                                 nullptr,
                                                 &semaphore);
  helper::ThrowIfNotSuccess(result,
                            "could not create semaphore");
}

lpe::vulkan::Semaphore::~Semaphore()
{
  if (device)
  {
    VkDevice().destroySemaphore(semaphore);
  }
}

lpe::vulkan::Fence::Fence(const Fence& other)
{
  fence = other.fence;
}

lpe::vulkan::Fence::Fence(Fence&& other) noexcept
{
  fence = other.fence;
}

lpe::vulkan::Fence& lpe::vulkan::Fence::operator=(const Fence& other)
{
  fence = other.fence;
  return *this;
}

lpe::vulkan::Fence& lpe::vulkan::Fence::operator=(Fence&& other) noexcept
{
  fence = other.fence;
  return *this;
}

lpe::vulkan::Fence::Fence(Instance* instance,
                          PhysicalDevice* physicalDevice,
                          Device* device,
                          bool signaled)
  : DeviceLevelObject(instance,
                      physicalDevice,
                      device)
{
  vk::FenceCreateInfo createInfo = {
    signaled ?
      vk::FenceCreateFlagBits::eSignaled :
      vk::FenceCreateFlags()
  };

  vk::Result result = VkDevice().createFence(&createInfo,
                                             nullptr,
                                             &fence);

  helper::ThrowIfNotSuccess(result,
                            "failed to create fence");

  Reset();
}

lpe::vulkan::Fence::~Fence()
{
  if(device)
  {
    VkDevice().destroyFence(fence);
  }
}

void lpe::vulkan::Fence::Wait(uint64_t timeout) const
{
  vk::Result result = VkDevice().waitForFences(1,
                                               &fence,
                                               VK_TRUE,
                                               timeout);

  // TODO: maybe not throw a exception?
  helper::ThrowIfNotSuccess(result,
                            "failed to wait for fence");
}

void lpe::vulkan::Fence::Reset()
{
  vk::Result result = VkDevice().resetFences(1,
                                             &fence);

  helper::ThrowIfNotSuccess(result,
                            "failed to reset for fence");
}
