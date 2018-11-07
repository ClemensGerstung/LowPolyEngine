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

  // maybe use this->manager instead?
  this->device = manager->GetDevice();

  if (!lpe::rendering::vulkan::common::CreateImageView(device,
                                                       image,
                                                       this->imageView,
                                                       this->viewType,
                                                       this->format,
                                                       this->aspectFlags,
                                                       this->baseMipLevel,
                                                       this->mipLevels,
                                                       this->baseLayer,
                                                       this->layers))
  {
    auto logPtr = logger.lock();
    if (logPtr)
    {
      logPtr->Log("Could not create ImageView. Check validation layers!");
    }

    return false;
  }

  return true;
}

lpe::rendering::vulkan::VulkanImage::VulkanImage()
{
  this->format = vk::Format::eUndefined;
  this->mipLevels = VK_REMAINING_MIP_LEVELS;
  this->baseMipLevel = 0;
  this->layers = VK_REMAINING_ARRAY_LAYERS;
  this->baseLayer = 0;
  this->samples = vk::SampleCountFlagBits::e1;
  this->tiling = vk::ImageTiling::eOptimal;
  this->usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
  this->type = vk::ImageType::e2D;
  this->viewType = vk::ImageViewType::e2D;
  this->aspectFlags = vk::ImageAspectFlagBits::eColor;
  this->extent = vk::Extent3D{
    1,
    1,
    1
  };
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

void lpe::rendering::vulkan::VulkanImage::Destroy()
{
  //auto renderer = manager.lock();

  if (imageView)
  {
    device.destroyImageView(imageView);
    imageView = nullptr;
  }

  if (image)
  {
    device.destroyImage(image);
    image = nullptr;
  }

  device = nullptr;
  manager.reset();
}

bool lpe::rendering::vulkan::VulkanImage::Create(std::shared_ptr<VulkanManager>&& manager,
                                                 std::weak_ptr<lpe::utils::Resource> resource,
                                                 int desiredChannels)
{
  const auto& ptr = resource.lock();
  if (!ptr)
  {
    return false;
  }

  const uint8_t *data;
  uint64_t size = ptr->GetData(&data);
  int width, height, channels;
  auto stbImage = stbi_load_from_memory(data, static_cast<int>(size), &width, &height, &channels, desiredChannels);

  assert(stbImage && width > 0 && height > 0 && channels > 0);

  vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(width * height * desiredChannels);

  extent.width = static_cast<uint32_t>(width);
  extent.height = static_cast<uint32_t>(height);

  this->Create(std::move(manager)); // TODO: still don't know if rvalue param is the best option. But nvm for now

  auto vulkan = this->manager.lock();

  auto allocator = vulkan->GetDeviceLocalMemory();

  assert(allocator.Fits(imageSize));

  auto offset = allocator.Push(stbImage, imageSize);

  stbi_image_free(stbImage);

  if (!lpe::rendering::vulkan::common::CreateImageView(device,
                                                       image,
                                                       this->imageView,
                                                       this->viewType,
                                                       this->format,
                                                       this->aspectFlags,
                                                       this->baseMipLevel,
                                                       this->mipLevels,
                                                       this->baseLayer,
                                                       this->layers))
  {
    auto logPtr = logger.lock();
    if (logPtr)
    {
      logPtr->Log("Could not create ImageView. Check validation layers!");
    }

    return false;
  }


  return true;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetWidth(uint32_t width)
{
  extent.width = width;

  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetHeight(uint32_t height)
{
  extent.height = height;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetDepth(uint32_t depth)
{
  extent.depth = depth;

  return *this;
}

bool lpe::rendering::vulkan::VulkanImage::Create(std::shared_ptr<lpe::rendering::vulkan::VulkanManager>&& manager)
{
  this->logger = lpe::ServiceLocator::LogManager.Get();
  this->manager = manager;
  this->device = manager->GetDevice();

  vk::ImageCreateInfo imageCreateInfo =
    {
      {},
      this->type,
      this->format,
      this->extent,
      this->baseLayer,
      this->layers,
      this->samples,
      this->tiling,
      this->usage,
      vk::SharingMode::eExclusive
    };

  auto result = device.createImage(&imageCreateInfo, nullptr, &this->image);

  if(result != vk::Result::eSuccess)
  {
    assert(false);

    auto ptr = logger.lock();
    if(ptr)
    {
      ptr->Log("Could not create Image");
    }

    return false;
  }

  return true;
}

bool lpe::rendering::vulkan::common::CreateImageView(vk::Device device,
                                                     vk::Image image,
                                                     vk::ImageView& view,
                                                     vk::ImageViewType viewType,
                                                     vk::Format format,
                                                     vk::ImageAspectFlags aspectFlags,
                                                     uint32_t baseMipLevel,
                                                     uint32_t mipLevels,
                                                     uint32_t baseArrayLayer,
                                                     uint32_t arrayLayers,
                                                     vk::ComponentSwizzle r,
                                                     vk::ComponentSwizzle g,
                                                     vk::ComponentSwizzle b,
                                                     vk::ComponentSwizzle a)
{
  vk::ImageViewCreateInfo createInfo = {
    {},
    image,
    viewType,
    format,
    {
      r,
      g,
      b,
      a
    },
    {
      aspectFlags,
      baseMipLevel,
      mipLevels,
      baseArrayLayer,
      arrayLayers
    }
  };

  auto result = device.createImageView(&createInfo, nullptr, &view);

  return result == vk::Result::eSuccess;
}
