#pragma once
#include <vulkan/vulkan.hpp>
#include "RenderObject.h"

namespace lpe
{
  namespace render
  {
    class VkTexture
    {
    protected:
      vk::Image image;
      vk::ImageView view;
      vk::DeviceMemory& memory;

      uint32_t requestedComponents;
    public:
      VkTexture() = default;
      VkTexture(const VkTexture& other);
      VkTexture(VkTexture&& other) noexcept;
      VkTexture& operator=(const VkTexture& other);
      VkTexture& operator=(VkTexture&& other) noexcept;
      virtual ~VkTexture() = default;

      void RequestedComponents(uint32_t channels);
      uint32_t RequestedComponents() const;

      virtual void Create(vk::Device device, const std::weak_ptr<Texture>& texture) = 0;
      virtual void Destroy(vk::Device device) = 0;
    };

    class VkTexture2D : public VkTexture
    {
    public:
      void Create(vk::Device device, const std::weak_ptr<Texture>& texture) override;
    };

    class VkCubeTexture : public VkTexture
    {
    public:
      void Create(vk::Device device, const std::weak_ptr<Texture>& texture) override;
    };
  }
}
