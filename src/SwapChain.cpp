#include "lpe.h"

lpe::vulkan::SwapChain::SwapChain(const SwapChain& other)
{
  this->surface = other.surface;
  this->presentMode = other.presentMode;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->format = other.format;
  this->colorSpace = other.colorSpace;
  this->swapChainImages = { other.swapChainImages };
}

lpe::vulkan::SwapChain::SwapChain(SwapChain&& other) noexcept
{
  this->surface = other.surface;
  this->presentMode = other.presentMode;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->format = other.format;
  this->colorSpace = other.colorSpace;
  this->swapChainImages = std::move(other.swapChainImages);
}

lpe::vulkan::SwapChain& lpe::vulkan::SwapChain::operator=(const SwapChain& other)
{
  this->surface = other.surface;
  this->presentMode = other.presentMode;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->format = other.format;
  this->colorSpace = other.colorSpace;
  this->swapChainImages = { other.swapChainImages };
  return *this;
}

lpe::vulkan::SwapChain& lpe::vulkan::SwapChain::operator=(SwapChain&& other) noexcept
{
  this->surface = other.surface;
  this->presentMode = other.presentMode;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->format = other.format;
  this->colorSpace = other.colorSpace;
  this->swapChainImages = std::move(other.swapChainImages);
  return *this;
}

lpe::vulkan::SwapChain::SwapChain(Instance* instance,
                                  PhysicalDevice* physicalDevice,
                                  Device* device,
                                  vk::SurfaceKHR surface,
                                  vk::PresentModeKHR presentMode,
                                  vk::Format format,
                                  vk::ColorSpaceKHR colorSpace)
  : DeviceLevelObject(instance,
                      physicalDevice,
                      device),
    surface(surface)
{
  std::vector<vk::PresentModeKHR> presentModes = VkPhysicalDevice().getSurfacePresentModesKHR(this->surface);

  // find the requested presentation mode
  // if it's not available use FIFO, this must be available
  auto iter = std::find(presentModes.begin(),
                        presentModes.end(),
                        presentMode);
  this->presentMode = iter != presentModes.end() ?
                        presentMode :
                        vk::PresentModeKHR::eFifo;

  this->physicalDevice->SupportsSurfaceFormat(&format,
                                              &colorSpace);

  this->format = format;
  this->colorSpace = colorSpace;

  Recreate();

  swapChainImages = VkDevice().getSwapchainImagesKHR(swapchain);
}

lpe::vulkan::SwapChain::~SwapChain()
{
  if(swapchain)
  {
    VkDevice().destroySwapchainKHR(swapchain);
    swapchain = nullptr;
  }
}

vk::Image lpe::vulkan::SwapChain::AquireNextImage(vk::Semaphore semaphore,
  vk::Fence fence, uint32_t *index)
{
  vk::Result result = VkDevice().acquireNextImageKHR(swapchain, 2000000000, semaphore, fence, index);

  if (result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR)
  {
    return swapChainImages[*index];
  }
  if(result == vk::Result::eErrorOutOfDateKHR)
  {
    Recreate();
    return AquireNextImage(semaphore, fence, index);
  }

  throw std::runtime_error("Could not get Image");
}

void lpe::vulkan::SwapChain::Present(uint32_t index, uint32_t presentQueueFamilyIndex, const std::vector<vk::Semaphore>& semaphores)
{
  vk::PresentInfoKHR presentInfo = { static_cast<uint32_t>(semaphores.size()), semaphores.data(), (uint32_t)1, &swapchain, &index };

  vk::Result result = device->GetQueue(presentQueueFamilyIndex).presentKHR(presentInfo);

  helper::ThrowIfNotSuccess(result, "could not present image");
}

void lpe::vulkan::SwapChain::Recreate()
{
  vk::SwapchainKHR oldSwapChain = swapchain;
  extent = this->physicalDevice->GetSwapChainImageExtent();

  vk::SwapchainCreateInfoKHR createInfo = {
    {},
    surface,
    physicalDevice->GetSwapChainImageCount(),
    format,
    colorSpace,
    extent,
    1,
    vk::ImageUsageFlagBits::eColorAttachment,
    vk::SharingMode::eExclusive,
    0,
    nullptr,
    physicalDevice->QuerySwapChainSupportDetails().capabilities.currentTransform,
    vk::CompositeAlphaFlagBitsKHR::eOpaque,
    this->presentMode,
    VK_TRUE,
    oldSwapChain
  };

  vk::Result result = VkDevice().createSwapchainKHR(&createInfo,
                                                     nullptr,
                                                     &swapchain);
  helper::ThrowIfNotSuccess(result,
                            "Could not create SwapChain");

  if(oldSwapChain)
  {
    VkDevice().destroySwapchainKHR(oldSwapChain);
  }
}
