#pragma once
#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace render
  {
    struct Attachment
    {
      vk::Image Image;
      uint32_t Index;
      vk::ImageLayout Layout;
      vk::AttachmentDescriptionFlags Flags;
      vk::Format Format;
      vk::SampleCountFlagBits Samples;
      vk::AttachmentLoadOp LoadOp;
      vk::AttachmentStoreOp StoreOp;
      vk::AttachmentLoadOp StencilLoadOp;
      vk::AttachmentStoreOp StencilStoreOp;
      vk::ImageLayout InitialLayout;
      vk::ImageLayout FinalLayout;

      Attachment() = default;
      Attachment(const Attachment& other) = default;
      Attachment(Attachment&& other) noexcept = default;
      Attachment& operator=(const Attachment& other) = default;
      Attachment& operator=(Attachment&& other) noexcept = default;

      Attachment(const vk::Image& image,
                 uint32_t index,
                 vk::ImageLayout layout,
                 const vk::AttachmentDescriptionFlags& flags,
                 vk::Format format,
                 vk::SampleCountFlagBits samples,
                 vk::AttachmentLoadOp loadOp,
                 vk::AttachmentStoreOp storeOp,
                 vk::AttachmentLoadOp stencilLoadOp,
                 vk::AttachmentStoreOp stencilStoreOp,
                 vk::ImageLayout initialLayout,
                 vk::ImageLayout finalLayout);

      ~Attachment() = default;

      vk::AttachmentReference GetAttachmentReference() const;
      vk::AttachmentDescription GetAttachmentDescription() const;

      operator bool() const;
      bool operator!() const;
    };


    struct SubpassParameters
    {
      std::vector<uint32_t> InputAttachmentIndices;
      std::vector<uint32_t> ColorAttachmentIndices;
      std::vector<uint32_t> ResolveAttachmentIndices;
      uint32_t DepthAttachmentIndices;
      vk::PipelineBindPoint BindPoint;
    };

    enum class RenderPassState
    {
      Creating,
      Recording,
      Ended
    };

    class RenderPass
    {
    private:
      std::vector<vk::SubpassDependency> subpassDependencies;
      std::vector<Attachment> attachments;
      std::vector<SubpassParameters> subpasses;

      vk::RenderPass renderPass;
      vk::Framebuffer currentFrameBuffer;
      vk::CommandBuffer currentCmdBuffer;
      RenderPassState state;
    public:
      RenderPass() = default;
      RenderPass(const RenderPass& other);
      RenderPass(RenderPass&& other) noexcept;
      RenderPass& operator=(const RenderPass& other);
      RenderPass& operator=(RenderPass&& other) noexcept;
      ~RenderPass();

      const Attachment& AddAttachment(const vk::Image& image,
                                      uint32_t index,
                                      vk::ImageLayout layout,
                                      const vk::AttachmentDescriptionFlags& flags,
                                      vk::Format format,
                                      vk::SampleCountFlagBits samples,
                                      vk::AttachmentLoadOp loadOp,
                                      vk::AttachmentStoreOp storeOp,
                                      vk::AttachmentLoadOp stencilLoadOp,
                                      vk::AttachmentStoreOp stencilStoreOp,
                                      vk::ImageLayout initialLayout,
                                      vk::ImageLayout finalLayout);
      void AddAttachment(const Attachment& attachment);
      void AddAttachment(Attachment&& attachment);

      const SubpassParameters& AddSubpass(vk::PipelineBindPoint bindPoint,
                                          std::vector<uint32_t> inputAttachments,
                                          std::vector<uint32_t> colorAttachments,
                                          std::vector<uint32_t> resolveAttachments,
                                          uint32_t depthAttachment);
      void AddSubpass(const SubpassParameters& attachment);
      void AddSubpass(SubpassParameters&& attachment);

      vk::SubpassDependency AddSubpassDependency(uint32_t srcSubpass_ = 0,
                                                 uint32_t dstSubpass_ = 0,
                                                 vk::PipelineStageFlags srcStageMask_ = vk::PipelineStageFlags(),
                                                 vk::PipelineStageFlags dstStageMask_ = vk::PipelineStageFlags(),
                                                 vk::AccessFlags srcAccessMask_ = vk::AccessFlags(),
                                                 vk::AccessFlags dstAccessMask_ = vk::AccessFlags(),
                                                 vk::DependencyFlags dependencyFlags_ = vk::DependencyFlags()) const;

      vk::RenderPass& Create(vk::Device device) const;
      vk::Framebuffer& CreateFrameBuffer(vk::Device device) const;
      bool Begin(vk::CommandBuffer cmdBuffer,
                 vk::Rect2D renderArea,
                 std::vector<vk::ClearValue> clearValues,
                 vk::SubpassContents contents);
      bool NextSubpass(vk::SubpassContents contents);
      bool End();
    };
  }
}
