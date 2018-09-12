#include "VkTexture.h"

void lpe::render::VkTexture2D::Create(vk::Device device,
                                      const std::weak_ptr<Texture>& texture)
{
  auto ptr = texture.lock();
  assert(ptr);
  
}
