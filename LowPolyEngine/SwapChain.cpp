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

void lpe::SwapChain::CreateImageViews()
{
  auto swapchainImages = device->getSwapchainImagesKHR(swapchain);

  imageViews.resize(swapchainImages.size());

  for (int i = 0; i < swapchainImages.size(); i++)
  {
    imageViews[i] = {device.get(), swapchainImages[i], imageFormat, vk::ImageAspectFlagBits::eColor};
  }
}

void lpe::SwapChain::Move(lpe::SwapChain& other)
{
  this->device.reset(other.device.get());
  other.device.release();
  this->physicalDevice = other.physicalDevice;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->imageFormat = other.imageFormat;
  this->imageViews = std::move(other.imageViews);
  this->framebuffers = std::move(other.framebuffers);
}

void lpe::SwapChain::Copy(const lpe::SwapChain& other)
{
  this->device.reset(other.device.get());
  
  this->physicalDevice = other.physicalDevice;
  this->swapchain = other.swapchain;
  this->extent = other.extent;
  this->imageFormat = other.imageFormat;
  this->imageViews = { other.imageViews };
  this->framebuffers = { other.framebuffers };
}

lpe::SwapChain::SwapChain(const SwapChain& other)
{
  Copy(other);
}

lpe::SwapChain::SwapChain(SwapChain&& other)
{
  Move(other);
}

lpe::SwapChain& lpe::SwapChain::operator=(const SwapChain& other)
{
  Copy(other);

  return *this;
}

lpe::SwapChain& lpe::SwapChain::operator=(SwapChain&& other)
{
  Move(other);

  return *this;
}

lpe::SwapChain::SwapChain(vk::PhysicalDevice physicalDevice,
                          std::unique_ptr<vk::Device> device,
                          const vk::SurfaceKHR& surface,
                          QueueFamilyIndices indices,
                          uint32_t width,
                          uint32_t height)
  : physicalDevice(physicalDevice)
{
  this->device.swap(device);

  CreateSwapChain(physicalDevice, surface, indices, width, height);

  CreateImageViews();
}

lpe::SwapChain::~SwapChain()
{
  if(device)
  {
    // this->imageViews will be automatically destroyed by the std::vector

    for (size_t i = 0; i < framebuffers.size(); ++i)
    {
      device->destroyFramebuffer(framebuffers[i]);
    }

    if(swapchain)
    {
      device->destroySwapchainKHR(swapchain, nullptr);
    }

    device.release();
  }
}

std::vector<vk::Framebuffer> lpe::SwapChain::CreateFrameBuffers(vk::RenderPass* renderPass, lpe::ImageView* depthImage)
{
  framebuffers.resize(imageViews.size());

  for (size_t i = 0; i < imageViews.size(); i++)
  {
    std::array<vk::ImageView, 2> attachments = { imageViews[i].GetImageView(), depthImage->GetImageView() };

    vk::FramebufferCreateInfo framebufferInfo = { {}, *renderPass, (uint32_t)attachments.size(), attachments.data(), extent.width, extent.height, 1 };

    auto result = device->createFramebuffer(&framebufferInfo, nullptr, &framebuffers[i]);
    helper::ThrowIfNotSuccess(result, "failed to create framebuffer!");
  }

  return framebuffers;
}

vk::Extent2D lpe::SwapChain::GetExtent() const
{
  return extent;
}

vk::Format lpe::SwapChain::GetImageFormat() const
{
  return imageFormat;
}

std::vector<vk::Framebuffer> lpe::SwapChain::GetFramebuffers() const
{
  return framebuffers;
}

vk::SwapchainKHR lpe::SwapChain::GetSwapchain() const
{
  return swapchain;
}
