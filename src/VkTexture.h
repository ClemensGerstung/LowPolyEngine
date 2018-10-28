#pragma once

#include <vulkan/vulkan.hpp>
#include "RenderObject.h"
#include "VkAttachment.h"

namespace lpe
{
  namespace rendering
  {
    class VkTexture
    {
    protected:
      vk::Device device;
      vk::Image image;
      vk::ImageView view;
      vk::DeviceMemory memory;

      uint32_t requestedComponents;
      vk::Format format;
      uint32_t level;
      uint32_t layers;
      vk::SampleCountFlagBits samples;
      vk::ImageTiling tiling;
      vk::ImageUsageFlags usage;
      Attachment attachment;
      vk::AccessFlags currentAccess;
      vk::ImageLayout currentLayout;
      uint32_t currentQueueFamilyIndex;
    public:
      VkTexture();

      VkTexture(const VkTexture &other);

      VkTexture(VkTexture &&other) noexcept;

      VkTexture &operator=(const VkTexture &other);

      VkTexture &operator=(VkTexture &&other) noexcept;

      virtual ~VkTexture() = default;

      VkTexture &RequestedComponents(uint32_t channels);

      uint32_t RequestedComponents() const;

      VkTexture &SetFormat(vk::Format format);

      VkTexture &SetMipLevels(uint32_t levels);

      virtual VkTexture &SetLayers(uint32_t layers);

      VkTexture &SetSamples(vk::SampleCountFlagBits samples);

      VkTexture &SetTiling(vk::ImageTiling tiling);

      VkTexture &SetUsage(vk::ImageUsageFlags usage);

      VkTexture &SetAccess(vk::AccessFlags access);

      VkTexture &SetLayout(vk::ImageLayout layout);

      VkTexture &SetQueueFamily(uint32_t queueFamilyIndex);

      virtual void Create(vk::Device device,
                          const std::weak_ptr<Texture> &texture) = 0;

      virtual void Destroy() = 0;

      void TransitionLayout(vk::CommandBuffer buffer,
                            vk::PipelineStageFlags srcStages,
                            vk::PipelineStageFlags dstStages,
                            vk::AccessFlags newAccess,
                            vk::ImageLayout newLayout,
                            uint32_t newQueueFamilyIndex,
                            vk::ImageAspectFlags aspect);

      Attachment &GetAttachment(uint32_t index,
                                vk::ImageLayout layout,
                                vk::AttachmentLoadOp loadOp,
                                vk::AttachmentStoreOp storeOp,
                                vk::AttachmentLoadOp stencilLoadOp,
                                vk::AttachmentStoreOp stencilStoreOp,
                                vk::ImageLayout initialLayout,
                                vk::ImageLayout finalLayout);
    };

    class VkTexture2D : public VkTexture
    {
    private:

    public:
      void Create(vk::Device device,
                  const std::weak_ptr<Texture> &texture) override;

      void Destroy() override;
    };

    class VkCubeTexture : public VkTexture
    {
    public:
      void Create(vk::Device device,
                  const std::weak_ptr<Texture> &texture) override;
    };
  }
}
