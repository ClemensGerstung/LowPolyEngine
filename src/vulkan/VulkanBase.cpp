#include "VulkanManager.hpp"
#include "../ServiceLocator.h"
#include <set>
#include "VulkanBase.hpp"

lpe::rendering::vulkan::VulkanBase & lpe::rendering::vulkan::VulkanBase::operator=(std::nullptr_t)
{
  instance = nullptr;
  physicalDevice = nullptr;
  surface = nullptr;

  return *this;
}

lpe::rendering::vulkan::VulkanQueue & lpe::rendering::vulkan::VulkanQueue::operator=(std::nullptr_t)
{
  queue = nullptr;
  queueFamilyIndex = 0;

  return *this;
}

lpe::rendering::vulkan::VulkanQueue::operator vk::Queue() const
{
  return queue;
}

bool lpe::rendering::vulkan::VulkanQueue::operator!() const
{
  return !queue;
}

lpe::rendering::vulkan::VulkanQueue & lpe::rendering::vulkan::VulkanQueue::operator=(vk::Queue queue)
{
  this->queue = queue;
  return *this;
}

lpe::rendering::vulkan::VulkanQueue::operator bool() const
{
  return (bool)queue;
}

lpe::rendering::vulkan::VulkanDevice & lpe::rendering::vulkan::VulkanDevice::operator=(std::nullptr_t)
{
  device = nullptr;
  graphicsQueue = nullptr;
  presentQueue = nullptr;
  computeQueue = nullptr;

  return *this;
}

lpe::rendering::vulkan::VulkanSwapchain& lpe::rendering::vulkan::VulkanSwapchain::operator=(std::nullptr_t)
{
  swapchain = nullptr;
  images.clear();

  return *this;
}