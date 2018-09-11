#pragma once
#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace render
  {
    struct Attachment
    {
      vk::ImageView ImageView;
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

      Attachment(const vk::ImageView& imageView,
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
      std::vector<uint32_t> PreserveAttachmentIndices;
      uint32_t DepthAttachmentIndex;
      vk::PipelineBindPoint BindPoint;
    };

    enum class RenderPassState
    {
      Creating,
      Created,
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

      void FillAttachments(std::vector<vk::AttachmentReference>& attachments,
                           const std::vector<uint32_t>& indices);
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

      vk::SubpassDependency AddSubpassDependency(uint32_t srcSubpass,
                                                 uint32_t dstSubpass,
                                                 vk::PipelineStageFlags srcStageMask,
                                                 vk::PipelineStageFlags dstStageMask,
                                                 vk::AccessFlags srcAccessMask,
                                                 vk::AccessFlags dstAccessMask,
                                                 vk::DependencyFlags dependencyFlags) const;

      const vk::RenderPass& Create(vk::Device device);
      const vk::Framebuffer& CreateFrameBuffer(vk::Device device,
                                               uint32_t width,
                                               uint32_t height,
                                               uint32_t layers);
      void Begin(vk::CommandBuffer cmdBuffer,
                 vk::Rect2D renderArea,
                 std::vector<vk::ClearValue> clearValues,
                 vk::SubpassContents contents);
      void NextSubpass(vk::SubpassContents contents);
      void End(vk::Device device);

      void Destroy(vk::Device device);
    };
  }
}
