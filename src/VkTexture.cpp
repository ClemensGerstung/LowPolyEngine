#include "VkTexture.h"
#include <stb_image.h>

void lpe::render::VkTexture2D::Create(vk::Device device,
                                      const std::weak_ptr<Texture>& texture)
{
  auto ptr = texture.lock();
  assert(ptr);

  auto image = ptr->GetImage().lock();
  assert(image);

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
    vk::Format::eR8G8B8A8Unorm,
    {width, height, 1},
    1,
    1,
    vk::SampleCountFlagBits::e1,
    vk::ImageTiling::eLinear,
    vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
    vk::SharingMode::eExclusive
  };

  auto result = device.createImage(&imageCreateInfo, nullptr, &this->image);

  assert(result == vk::Result::eSuccess);

  stbi_image_free(stbImage);
}
