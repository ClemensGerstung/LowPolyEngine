#include "SwapChain.h"
#include "Instance.h"

vk::SurfaceFormatKHR lpe::SwapChain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const
{
  if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
  {
    return{ vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
  }

  for (const auto& availableFormat : formats)
  {
    if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
    {
      return availableFormat;
    }
  }

  return formats[0];
}

vk::PresentModeKHR lpe::SwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes) const
{
  for (const auto& presentMode : presentModes)
  {
    if (presentMode == vk::PresentModeKHR::eMailbox)
    {
      return presentMode;
    }
  }

  return vk::PresentModeKHR::eImmediate;
}

vk::Extent2D lpe::SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,
                                              const uint32_t width,
                                              const uint32_t height) const
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
  {
    return capabilities.currentExtent;
  }

  vk::Extent2D actualExtent = { width, height };

  actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
  actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

  return actualExtent;
}

void lpe::SwapChain::CreateSwapChain(vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR& surface, lpe::QueueFamilyIndices indices, uint32_t width, uint32_t height)
{
  auto details = Instance::QuerySwapChainDetails(physicalDevice, surface);

  vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
  vk::PresentModeKHR presentMode = ChooseSwapPresentMode(details.presentModes);
  vk::Extent2D extent = ChooseSwapExtent(details.capabilities, width, height);

  uint32_t imageCount = details.capabilities.minImageCount + 1;
  if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
  {
    imageCount = details.capabilities.maxImageCount;
  }

  vk::SwapchainKHR oldSwapChain = swapchain;
  vk::SwapchainCreateInfoKHR createInfo =
  {
    {},
    surface,
    imageCount,
    surfaceFormat.format,
    surfaceFormat.colorSpace,
    extent,
    1,
    vk::ImageUsageFlagBits::eColorAttachment,
    vk::SharingMode::eExclusive,
    0,
    nullptr,
    details.capabilities.currentTransform,
    vk::CompositeAlphaFlagBitsKHR::eOpaque,
    presentMode,
    VK_TRUE,
    oldSwapChain
  };

  auto queueFamilyIndices = indices.GetAsArray();

  if (indices.graphicsFamily != indices.presentFamily)
  {
    createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    createInfo.queueFamilyIndexCount = (uint32_t)queueFamilyIndices.size();
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  }

  vk::SwapchainKHR newSwapchain = device->createSwapchainKHR(createInfo, nullptr);
  swapchain = newSwapchain;

  imageFormat = surfaceFormat.format;
  this->extent = extent;
}

lpe::SwapChain::SwapChain(const SwapChain& other)
{
  this->device.reset(other.device.get());
  
  this->physicalDevice = other.physicalDevice;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->imageFormat = other.imageFormat;
}

lpe::SwapChain::SwapChain(SwapChain&& other)
{
  this->device.reset(other.device.get());
  other.device.release();
  this->physicalDevice = other.physicalDevice;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->imageFormat = other.imageFormat;
}

lpe::SwapChain& lpe::SwapChain::operator=(const SwapChain& other)
{
  this->device.reset(other.device.get());

  this->physicalDevice = other.physicalDevice;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->imageFormat = other.imageFormat;

  return *this;
}

lpe::SwapChain& lpe::SwapChain::operator=(SwapChain&& other)
{
  this->device.reset(other.device.get());
  other.device.release();
  this->physicalDevice = other.physicalDevice;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->imageFormat = other.imageFormat;

  return *this;
}

lpe::SwapChain::SwapChain(vk::PhysicalDevice physicalDevice,
                          vk::Device* device,
                          const vk::SurfaceKHR& surface,
                          QueueFamilyIndices indices,
                          uint32_t width,
                          uint32_t height)
  : physicalDevice(physicalDevice)
{
  this->device.reset(device);

  CreateSwapChain(physicalDevice, surface, indices, width, height);
}

lpe::SwapChain::~SwapChain()
{
  if(device)
  {
    

    if(swapchain)
    {
      device->destroySwapchainKHR(swapchain, nullptr);
    }

    device.release();
  }
}
