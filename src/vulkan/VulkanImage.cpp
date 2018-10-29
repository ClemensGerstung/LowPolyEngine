#include "VulkanImage.hpp"
#include "VulkanManager.hpp"
#include "../ServiceLocator.h"

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetFormat(vk::Format format)
{
  VulkanImage::format = format;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetLevel(uint32_t level)
{
  VulkanImage::mipLevels = level;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetLayers(uint32_t layers)
{
  VulkanImage::layers = layers;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetSamples(vk::SampleCountFlagBits samples)
{
  VulkanImage::samples = samples;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetTiling(vk::ImageTiling tiling)
{
  VulkanImage::tiling = tiling;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetUsage(const vk::ImageUsageFlags& usage)
{
  VulkanImage::usage = usage;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetType(vk::ImageType type)
{
  VulkanImage::type = type;
  return *this;
}

bool lpe::rendering::vulkan::VulkanImage::Create(std::shared_ptr<lpe::rendering::vulkan::VulkanManager>&& manager,
                                                 vk::Image image)
{
  this->logger = lpe::ServiceLocator::LogManager.Get();
  this->manager = manager;
  this->image = image;
  vk::ImageViewCreateInfo createInfo = {
    {},
    image,
    this->viewType,
    this->format,
    {
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity,
      vk::ComponentSwizzle::eIdentity
    },
    {
      this->aspectFlags,
      this->baseMipLevel,
      this->mipLevels,
      this->baseLayer,
      this->layers
    }
  };

  // maybe use this->manager instead?
  auto device = manager->GetDevice();
  auto result = device.createImageView(&createInfo, nullptr, &this->imageView);

  if(result != vk::Result::eSuccess)
  {
    auto logPtr = logger.lock();
    if(logPtr)
    {
      logPtr->Log("Could not create ImageView. Check validation layers! Result: " + vk::to_string(result));
    }

    return false;
  }

  return true;
}

lpe::rendering::vulkan::VulkanImage::VulkanImage()
{
  this->format = vk::Format::eUndefined;
  this->mipLevels = 1;
  this->baseMipLevel = 0;
  this->layers = 1;
  this->baseLayer = 0;
  this->samples = vk::SampleCountFlagBits::e1;
  this->tiling = vk::ImageTiling::eOptimal;
  this->usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
  this->type = vk::ImageType::e2D;
  this->viewType = vk::ImageViewType::e2D;
  this->aspectFlags = vk::ImageAspectFlagBits::eColor;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetMipLevels(uint32_t mipLevels)
{
  VulkanImage::mipLevels = mipLevels;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetBaseMipLevel(uint32_t baseMipLevel)
{
  VulkanImage::baseMipLevel = baseMipLevel;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetBaseLayer(uint32_t baseLayer)
{
  VulkanImage::baseLayer = baseLayer;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetViewType(vk::ImageViewType viewType)
{
  VulkanImage::viewType = viewType;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetAspectFlags(const vk::ImageAspectFlags& aspectFlags)
{
  VulkanImage::aspectFlags = aspectFlags;
  return *this;
}
