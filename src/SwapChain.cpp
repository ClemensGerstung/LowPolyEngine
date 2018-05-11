#include "lpe.h"

lpe::vulkan::SwapChain::SwapChain(const SwapChain& other)
{
}

lpe::vulkan::SwapChain::SwapChain(SwapChain&& other) noexcept
{
}

lpe::vulkan::SwapChain& lpe::vulkan::SwapChain::operator=(const SwapChain& other)
{
  return *this;
}

lpe::vulkan::SwapChain& lpe::vulkan::SwapChain::operator=(SwapChain&& other) noexcept
{
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
  std::vector<vk::PresentModeKHR> presentModes = this->GetPhysicalDevice().getSurfacePresentModesKHR(this->surface);

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

  swapChainImages = GetDevice().getSwapchainImagesKHR(swapchain);
}

lpe::vulkan::SwapChain::~SwapChain()
{
  if(swapchain)
  {
    GetDevice().destroySwapchainKHR(swapchain);
    swapchain = nullptr;
  }
}

vk::Image lpe::vulkan::SwapChain::AquireNextImage(vk::Semaphore semaphore,
  vk::Fence fence)
{
  uint32_t index;
  vk::Result result = GetDevice().acquireNextImageKHR(swapchain, 2000000000, semaphore, fence, &index);

  if (result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR)
  {
    return swapChainImages[index];
  }
  if(result == vk::Result::eErrorOutOfDateKHR)
  {
    Recreate();
    return AquireNextImage(semaphore, fence);
  }

  throw std::runtime_error("Could not get Image");
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

  vk::Result result = GetDevice().createSwapchainKHR(&createInfo,
                                                     nullptr,
                                                     &swapchain);
  helper::ThrowIfNotSuccess(result,
                            "Could not create SwapChain");

  if(oldSwapChain)
  {
    GetDevice().destroySwapchainKHR(oldSwapChain);
  }
}
