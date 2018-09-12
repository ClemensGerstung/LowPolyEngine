#pragma once
#include <vulkan/vulkan.hpp>
#include "Resource.h"
#include "RenderObject.h"

namespace lpe
{
  namespace render
  {
    class VkTexture
    {
    private:
      vk::Image image;
      vk::ImageView view;
      vk::DeviceMemory& memory;
    public:
      VkTexture() = default;
      VkTexture(const VkTexture& other);
      VkTexture(VkTexture&& other) noexcept;
      VkTexture& operator=(const VkTexture& other);
      VkTexture& operator=(VkTexture&& other) noexcept;
      virtual ~VkTexture() = default;

      virtual void Create(vk::Device device, const std::weak_ptr<Texture>& texture) = 0;
    };

    class VkTexture2D : public VkTexture
    {
    public:
      void Create(vk::Device device, const std::weak_ptr<Texture>& texture) override;
    };
  }
}
