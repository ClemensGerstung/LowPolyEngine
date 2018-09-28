#include "VkTexture.h"
#include <stb_image.h>

void lpe::render::VkTexture2D::Create(vk::Device device,
  const std::weak_ptr<Texture>& texture)
{
  auto ptr = texture.lock();
  assert(ptr);

  auto image = ptr->GetImage().lock();
  if (image)
  {
    int width, height, channels;
    unsigned char const* data;
    uint64_t size = image->GetData(&data);

    auto stbImage = stbi_load_from_memory(data,
      size,
      &width,
      &height,
      &channels,
      requestedComponents);

    assert(stbImage && width > 0 && height > 0 && channels > 0);

    vk::ImageCreateInfo imageCreateInfo =
    {
      {},
      vk::ImageType::e2D,
      this->format,
      {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        1
      },
      this->level,
      this->layers,
      this->samples,
      this->tiling,
      this->usage,
      vk::SharingMode::eExclusive
    };

    auto result = device.createImage(&imageCreateInfo,
      nullptr,
      &this->image);

    assert(result == vk::Result::eSuccess);


    stbi_image_free(stbImage);
  }
  else
  {

  }
}

void lpe::render::VkTexture::TransitionLayout(vk::CommandBuffer buffer,
  vk::PipelineStageFlags srcStages,
  vk::PipelineStageFlags dstStages,
  vk::AccessFlags newAccess,
  vk::ImageLayout newLayout,
  uint32_t newQueueFamilyIndex,
  vk::ImageAspectFlags aspect)
{
  vk::ImageMemoryBarrier barrier = {
    this->currentAccess,
    newAccess,
    currentLayout,
    newLayout,
    currentQueueFamilyIndex,
    newQueueFamilyIndex,
    this->image,
    {
      aspect,
      0,
      VK_REMAINING_MIP_LEVELS,
      0,
      VK_REMAINING_ARRAY_LAYERS
    }
  };

  buffer.pipelineBarrier(srcStages, 
    dstStages,
    vk::DependencyFlags(),
    0,
    nullptr,
    0,
    nullptr,
    1,
    &barrier);
}

lpe::render::Attachment & lpe::render::VkTexture::GetAttachment(uint32_t index,
  vk::ImageLayout layout,
  vk::AttachmentLoadOp loadOp,
  vk::AttachmentStoreOp storeOp,
  vk::AttachmentLoadOp stencilLoadOp,
  vk::AttachmentStoreOp stencilStoreOp,
  vk::ImageLayout initialLayout,
  vk::ImageLayout finalLayout)
{
  this->attachment.ImageView = view;
  this->attachment.Index = index;
  this->attachment.Layout = layout;
  this->attachment.Flags = {};
  this->attachment.Format = this->format;
  this->attachment.Samples = this->samples;
  this->attachment.LoadOp = loadOp;
  this->attachment.StoreOp = storeOp;
  this->attachment.StencilLoadOp = stencilLoadOp;
  this->attachment.StencilStoreOp = stencilStoreOp;
  this->attachment.InitialLayout = initialLayout;
  this->attachment.FinalLayout = finalLayout;

  return this->attachment;
}
