#include "ImageView.h"

lpe::ImageView::ImageView(const ImageView& other)
{
  this->device.reset(other.device.get());
  this->image = other.image;
  this->imageView = other.imageView;
  this->memory = other.memory;
}

lpe::ImageView::ImageView(ImageView&& other)
{
  this->device = std::move(other.device);
  this->image = other.image;
  this->imageView = other.imageView;
  this->memory = other.memory;
}

lpe::ImageView& lpe::ImageView::operator=(const ImageView& other)
{
  this->device.reset(other.device.get());
  this->image = other.image;
  this->imageView = other.imageView;
  this->memory = other.memory;

  return *this;
}

lpe::ImageView& lpe::ImageView::operator=(ImageView&& other)
{
  this->device = std::move(other.device);
  this->image = other.image;
  this->imageView = other.imageView;
  this->memory = other.memory;

  return *this;
}

lpe::ImageView::ImageView(vk::Device* device,
                          const vk::Image& image,
                          vk::Format format,
                          vk::ImageAspectFlags flags)
{
  this->device.reset(device);

  vk::ImageViewCreateInfo createInfo = { {}, image, vk::ImageViewType::e2D, format,{},{ flags, 0, 1, 0, 1 } };
  auto result = device->createImageView(&createInfo, nullptr, &imageView);
  helper::ThrowIfNotSuccess(result, "failed to create texture image view!");
}

lpe::ImageView::ImageView(vk::PhysicalDevice physicalDevice,
                          vk::Device* device,
                          uint32_t width,
                          uint32_t height,
                          vk::Format format,
                          vk::ImageTiling tiling,
                          vk::ImageUsageFlags usage,
                          vk::MemoryPropertyFlags properties,
                          vk::ImageAspectFlags flags)
{
  this->device.reset(device);

  vk::ImageCreateInfo createInfo =
  {
    {},
    vk::ImageType::e2D,
    format,
    { width, height, 1 },
    1,
    1,
    vk::SampleCountFlagBits::e1,
    tiling,
    usage,
    vk::SharingMode::eExclusive,
    0,
    nullptr,
    vk::ImageLayout::eUndefined
  };

  auto result = this->device->createImage(&createInfo, nullptr, &image);
  helper::ThrowIfNotSuccess(result, "failed to create image!");

  vk::MemoryRequirements requirements = this->device->getImageMemoryRequirements(image);

  vk::MemoryAllocateInfo allocInfo = { requirements.size, helper::FindMemoryTypeIndex(requirements.memoryTypeBits, properties, physicalDevice.getMemoryProperties()) };

  result = this->device->allocateMemory(&allocInfo, nullptr, &memory);
  helper::ThrowIfNotSuccess(result, "failed to allocate image memory!");

  this->device->bindImageMemory(image, memory, 0);

  vk::ImageViewCreateInfo viewCreateInfo = { {}, image, vk::ImageViewType::e2D, format, {}, { flags, 0, 1, 0, 1 } };
  result = this->device->createImageView(&viewCreateInfo, nullptr, &imageView);
  helper::ThrowIfNotSuccess(result, "failed to create texture image view!");
}

lpe::ImageView::~ImageView()
{
  if(device)
  {
    if(imageView)
    {
      device->destroyImageView(imageView, nullptr);
    }

    if(image)
    {
      device->destroyImage(image, nullptr);
    }

    if(memory)
    {
      device->freeMemory(memory);
    }

    device.release();
  }
}

void lpe::ImageView::TransitionImageLayout(vk::CommandBuffer commandBuffer,
                                           vk::Format format,
                                           vk::ImageLayout oldLayout,
                                           vk::ImageLayout newLayout)
{
  vk::ImageSubresourceRange range = { {}, 0, 1, 0, 1 };
  vk::AccessFlags srcAccessMask;
  vk::AccessFlags dstAccessMask;
  vk::PipelineStageFlags sourceStage;
  vk::PipelineStageFlags destinationStage;

  if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
  {
    range.aspectMask = vk::ImageAspectFlagBits::eDepth;
    if (format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint)
    {
      range.aspectMask |= vk::ImageAspectFlagBits::eStencil;
    }
  }
  else
  {
    range.aspectMask = vk::ImageAspectFlagBits::eColor;
  }

  if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
  {
    srcAccessMask = {};
    dstAccessMask = vk::AccessFlagBits::eTransferWrite;

    sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    destinationStage = vk::PipelineStageFlagBits::eTransfer;
  }
  else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
  {
    srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    dstAccessMask = vk::AccessFlagBits::eShaderRead;

    sourceStage = vk::PipelineStageFlagBits::eTransfer;
    destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
  }
  else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
  {
    srcAccessMask = {};
    dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

    sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
  }
  else
  {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vk::ImageMemoryBarrier barrier = { srcAccessMask, dstAccessMask, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, range };

  commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1, &barrier);
}

vk::ImageView lpe::ImageView::GetImageView() const
{
  return imageView;
}
