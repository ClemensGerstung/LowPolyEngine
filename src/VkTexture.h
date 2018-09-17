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
      vk::Format format;
      uint32_t level;
      uint32_t layers;
      vk::SampleCountFlagBits samples;
      vk::ImageTiling tiling;
      vk::ImageUsageFlags usage;
    public:
      VkTexture();
      VkTexture(const VkTexture& other);
      VkTexture(VkTexture&& other) noexcept;
      VkTexture& operator=(const VkTexture& other);
      VkTexture& operator=(VkTexture&& other) noexcept;
      virtual ~VkTexture() = default;

      void RequestedComponents(uint32_t channels);
      uint32_t RequestedComponents() const;

      virtual VkTexture& SetFormat(vk::Format format);
      virtual VkTexture& SetMipLevels(uint32_t levels);
      virtual VkTexture& SetLayers(uint32_t layers);
      virtual VkTexture& SetSamples(vk::SampleCountFlagBits samples);
      virtual VkTexture& SetTiling(vk::ImageTiling tiling);
      virtual VkTexture& SetUsage(vk::ImageUsageFlags usage);

      virtual void Create(vk::Device device,
                          const std::weak_ptr<Texture>& texture) = 0;
      virtual void Destroy(vk::Device device) = 0;
    };

    class VkTexture2D : public VkTexture
    {
    private:

    public:
      void Create(vk::Device device,
                  const std::weak_ptr<Texture>& texture) override;

      void Destroy(vk::Device device) override;
    };

    class VkCubeTexture : public VkTexture
    {
    public:
      void Create(vk::Device device,
                  const std::weak_ptr<Texture>& texture) override;
    };
  }
}
