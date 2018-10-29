#ifndef LOWPOLYENGINE_VULKANBASE_HPP
#define LOWPOLYENGINE_VULKANBASE_HPP

#include <vulkan/vulkan.hpp>
#include "VulkanImage.hpp"

namespace lpe
{
namespace rendering
{
namespace vulkan
{

struct VulkanBase
{
  vk::Instance instance;
  vk::PhysicalDevice physicalDevice;
  struct VkSurfaceKHR_T *surface; // using default Vulkan Handle because it's easier in this case...

  VulkanBase& operator=(std::nullptr_t);
};

struct VulkanQueue
{
  vk::Queue queue;
  unsigned int queueFamilyIndex;

  VulkanQueue& operator=(std::nullptr_t);

  VulkanQueue& operator=(vk::Queue queue);

  explicit operator vk::Queue() const;

  bool operator!() const;

  explicit operator bool() const;
};

struct VulkanDevice
{
  vk::Device device;
  VulkanQueue graphicsQueue;
  VulkanQueue presentQueue;
  VulkanQueue computeQueue;

  VulkanDevice& operator=(std::nullptr_t);
};

struct VulkanSwapchain
{
  vk::SwapchainKHR swapchain;
  vk::SurfaceFormatKHR format;
  vk::Extent2D extent;
  vk::PresentModeKHR presentMode;
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<VulkanImage> images;

  VulkanSwapchain& operator=(std::nullptr_t);
};

}
}
}

#endif //LOWPOLYENGINE_VULKANBASE_HPP
