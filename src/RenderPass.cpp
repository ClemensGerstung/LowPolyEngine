#include "RenderPass.h"

lpe::render::Attachment::Attachment(const vk::ImageView& imageView,
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
                                    vk::ImageLayout finalLayout)
  : ImageView(imageView),
    Index(index),
    Layout(layout),
    Flags(flags),
    Format(format),
    Samples(samples),
    LoadOp(loadOp),
    StoreOp(storeOp),
    StencilLoadOp(stencilLoadOp),
    StencilStoreOp(stencilStoreOp),
    InitialLayout(initialLayout),
    FinalLayout(finalLayout)
{
}

vk::AttachmentReference lpe::render::Attachment::GetAttachmentReference() const
{
  return { Index, Layout };
}

vk::AttachmentDescription lpe::render::Attachment::GetAttachmentDescription() const
{
  return { Flags, Format, Samples, LoadOp, StoreOp, StencilLoadOp, StencilStoreOp, InitialLayout, FinalLayout };
}

lpe::render::Attachment::operator bool() const
{
  return ImageView && Flags;
}

bool lpe::render::Attachment::operator!() const
{
  return !(ImageView && Flags);
}

lpe::render::SubpassParameters::SubpassParameters(const std::vector<uint32_t>& inputAttachmentIndices,
                                                  const std::vector<uint32_t>& colorAttachmentIndices,
                                                  const std::vector<uint32_t>& resolveAttachmentIndices,
                                                  const std::vector<uint32_t>& preserveAttachmentIndices,
                                                  uint32_t depthAttachmentIndex,
                                                  vk::PipelineBindPoint bindPoint)
  : InputAttachmentIndices(inputAttachmentIndices),
    ColorAttachmentIndices(colorAttachmentIndices),
    ResolveAttachmentIndices(resolveAttachmentIndices),
    PreserveAttachmentIndices(preserveAttachmentIndices),
    DepthAttachmentIndex(depthAttachmentIndex),
    BindPoint(bindPoint)
{
}

lpe::render::SubpassParameters::SubpassParameters(const SubpassParameters& other)
  : InputAttachmentIndices(other.InputAttachmentIndices),
    ColorAttachmentIndices(other.ColorAttachmentIndices),
    ResolveAttachmentIndices(other.ResolveAttachmentIndices),
    PreserveAttachmentIndices(other.PreserveAttachmentIndices),
    DepthAttachmentIndex(other.DepthAttachmentIndex),
    BindPoint(other.BindPoint)
{
}

lpe::render::SubpassParameters::SubpassParameters(SubpassParameters&& other) noexcept
  : InputAttachmentIndices(std::move(other.InputAttachmentIndices)),
    ColorAttachmentIndices(std::move(other.ColorAttachmentIndices)),
    ResolveAttachmentIndices(std::move(other.ResolveAttachmentIndices)),
    PreserveAttachmentIndices(std::move(other.PreserveAttachmentIndices)),
    DepthAttachmentIndex(other.DepthAttachmentIndex),
    BindPoint(other.BindPoint)
{
}

lpe::render::SubpassParameters& lpe::render::SubpassParameters::operator=(const SubpassParameters& other)
{
  if (this == &other) return *this;
  InputAttachmentIndices = other.InputAttachmentIndices;
  ColorAttachmentIndices = other.ColorAttachmentIndices;
  ResolveAttachmentIndices = other.ResolveAttachmentIndices;
  PreserveAttachmentIndices = other.PreserveAttachmentIndices;
  DepthAttachmentIndex = other.DepthAttachmentIndex;
  BindPoint = other.BindPoint;
  return *this;
}

lpe::render::SubpassParameters& lpe::render::SubpassParameters::operator=(SubpassParameters&& other) noexcept
{
  if (this == &other) return *this;
  InputAttachmentIndices = std::move(other.InputAttachmentIndices);
  ColorAttachmentIndices = std::move(other.ColorAttachmentIndices);
  ResolveAttachmentIndices = std::move(other.ResolveAttachmentIndices);
  PreserveAttachmentIndices = std::move(other.PreserveAttachmentIndices);
  DepthAttachmentIndex = other.DepthAttachmentIndex;
  BindPoint = other.BindPoint;
  return *this;
}

void lpe::render::RenderPass::FillAttachments(std::vector<vk::AttachmentReference>& attachments,
                                              const std::vector<uint32_t>& indices)
{
  for (auto&& inputIndex : indices)
  {
    auto attachment = this->attachments[inputIndex];

    if (attachment.Index != inputIndex)
    {
      auto iterator = std::find_if(std::begin(this->attachments),
                                   std::end(this->attachments),
                                   [index = inputIndex](const Attachment& attachment)
                                   {
                                     return attachment.Index == index;
                                   });

      if (iterator != std::end(this->attachments))
      {
        attachment = *iterator;
      }
    }

    attachments.emplace_back(attachment.GetAttachmentReference());
  }
}

lpe::render::RenderPass::RenderPass()
{
  subpassDependencies = {};
  attachments = {};
  subpasses = {};
  renderPass = nullptr;
  currentFrameBuffer = nullptr;
  currentCmdBuffer = nullptr;
  state = RenderPassState::Creating;
}

lpe::render::RenderPass::RenderPass(const RenderPass& other)
{
  this->operator=(other);
}

lpe::render::RenderPass::RenderPass(RenderPass&& other) noexcept
{
  this->operator=(std::move(other));
}

lpe::render::RenderPass& lpe::render::RenderPass::operator=(const RenderPass& other)
{
  this->subpassDependencies = { other.subpassDependencies };
  this->attachments = { other.attachments };
  this->subpasses = { other.subpasses };

  this->renderPass = other.renderPass;
  this->currentFrameBuffer = other.currentFrameBuffer;
  this->currentCmdBuffer = other.currentCmdBuffer;
  this->state = other.state;

  return *this;
}

lpe::render::RenderPass& lpe::render::RenderPass::operator=(RenderPass&& other) noexcept
{
  this->subpassDependencies = std::move(other.subpassDependencies);
  this->attachments = std::move(other.attachments);
  this->subpasses = std::move(other.subpasses);

  this->renderPass = other.renderPass;
  this->currentFrameBuffer = other.currentFrameBuffer;
  this->currentCmdBuffer = other.currentCmdBuffer;
  this->state = other.state;

  return *this;
}

void lpe::render::RenderPass::AddAttachment(const vk::ImageView& view,
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
                                            vk::ImageLayout finalLayout)
{
  attachments.emplace_back(view,
                           index,
                           layout,
                           flags,
                           format,
                           samples,
                           loadOp,
                           storeOp,
                           stencilLoadOp,
                           stencilStoreOp,
                           initialLayout,
                           finalLayout);
}

void lpe::render::RenderPass::AddAttachment(const Attachment& attachment)
{
  attachments.push_back(attachment);
}

void lpe::render::RenderPass::AddAttachment(Attachment&& attachment)
{
  attachments.push_back(attachment);
}

void lpe::render::RenderPass::AddSubpass(vk::PipelineBindPoint bindPoint,
                                         std::vector<uint32_t> inputAttachments,
                                         std::vector<uint32_t> colorAttachments,
                                         std::vector<uint32_t> resolveAttachments,
                                         uint32_t depthAttachment,
                                         std::vector<uint32_t> preserveAttachments)
{
  subpasses.emplace_back(inputAttachments,
                         colorAttachments,
                         resolveAttachments,
                         preserveAttachments,
                         depthAttachment,
                         bindPoint);
}

void lpe::render::RenderPass::AddSubpass(const SubpassParameters& attachment)
{
  subpasses.push_back(attachment);
}

void lpe::render::RenderPass::AddSubpass(SubpassParameters&& attachment)
{
  subpasses.push_back(attachment);
}

void lpe::render::RenderPass::AddSubpassDependency(uint32_t srcSubpass,
                                                   uint32_t dstSubpass,
                                                   vk::PipelineStageFlags srcStageMask,
                                                   vk::PipelineStageFlags dstStageMask,
                                                   vk::AccessFlags srcAccessMask,
                                                   vk::AccessFlags dstAccessMask,
                                                   vk::DependencyFlags dependencyFlags)
{
  subpassDependencies.emplace_back(srcSubpass,
                                   dstSubpass,
                                   srcStageMask,
                                   dstStageMask,
                                   srcAccessMask,
                                   dstAccessMask,
                                   dependencyFlags);
}

const vk::RenderPass& lpe::render::RenderPass::Create(vk::Device device)
{
  assert(state == RenderPassState::Creating);

  std::vector<vk::AttachmentReference> inputAttachments;
  std::vector<vk::AttachmentReference> colorAttachments;
  std::vector<vk::AttachmentReference> resolveAttachments;
  vk::AttachmentReference depthAttachment;

  std::vector<vk::SubpassDescription> subpassDescriptions;
  std::vector<vk::AttachmentDescription> attachmentDescriptions;

  attachmentDescriptions.resize(attachments.size());

  for (auto&& subpass : subpasses)
  {
    inputAttachments.clear();
    colorAttachments.clear();
    resolveAttachments.clear();
    depthAttachment = VK_NULL_HANDLE;

    auto attachment = attachments[subpass.DepthAttachmentIndex];
    if (attachment.Index != subpass.DepthAttachmentIndex)
    {
      auto iterator = std::find_if(std::begin(attachments),
                                   std::end(attachments),
                                   [index = subpass.DepthAttachmentIndex](const Attachment& attachment)
                                   {
                                     return attachment.Index == index;
                                   });

      if (iterator != std::end(attachments))
      {
        attachment = *iterator;
      }
    }
    depthAttachment = attachment.GetAttachmentReference();

    FillAttachments(inputAttachments,
                    subpass.InputAttachmentIndices);
    FillAttachments(colorAttachments,
                    subpass.ColorAttachmentIndices);
    FillAttachments(resolveAttachments,
                    subpass.ResolveAttachmentIndices);

    subpassDescriptions.emplace_back(vk::SubpassDescriptionFlags(),
                                     subpass.BindPoint,
                                     static_cast<uint32_t>(inputAttachments.size()),
                                     inputAttachments.data(),
                                     static_cast<uint32_t>(colorAttachments.size()),
                                     colorAttachments.data(),
                                     resolveAttachments.data(),
                                     &depthAttachment,
                                     static_cast<uint32_t>(subpass.PreserveAttachmentIndices.size()),
                                     subpass.PreserveAttachmentIndices.data());
  }

  for (uint32_t i = 0; i < attachments.size(); ++i)
  {
    auto attachment = attachments[i];
    if (attachment.Index != i)
    {
      auto iterator = std::find_if(std::begin(this->attachments),
                                   std::end(this->attachments),
                                   [index = i](const Attachment& attachment)
                                   {
                                     return attachment.Index == index;
                                   });

      if (iterator != std::end(this->attachments))
      {
        attachment = *iterator;
      }
    }

    attachmentDescriptions[i] = attachment.GetAttachmentDescription();
  }

  vk::RenderPassCreateInfo createInfo =
  {
    {},
    static_cast<uint32_t>(attachmentDescriptions.size()),
    attachmentDescriptions.data(),
    static_cast<uint32_t>(subpassDescriptions.size()),
    subpassDescriptions.data(),
    static_cast<uint32_t>(subpassDependencies.size()),
    subpassDependencies.data()
  };

  auto result = device.createRenderPass(&createInfo,
                                        nullptr,
                                        &renderPass);

  assert(result == vk::Result::eSuccess);

  state = RenderPassState::Created;

  return renderPass;
}

const vk::Framebuffer& lpe::render::RenderPass::CreateFrameBuffer(vk::Device device,
                                                                  uint32_t width,
                                                                  uint32_t height,
                                                                  uint32_t layers)
{
  assert(state == RenderPassState::Created && renderPass);

  std::vector<vk::ImageView> views;
  views.resize(attachments.size());

  for (uint32_t i = 0; i < attachments.size(); ++i)
  {
    auto attachment = attachments[i];
    if (attachment.Index != i)
    {
      auto iterator = std::find_if(std::begin(this->attachments),
                                   std::end(this->attachments),
                                   [index = i](const Attachment& attachment)
                                   {
                                     return attachment.Index == index;
                                   });

      if (iterator != std::end(this->attachments))
      {
        attachment = *iterator;
      }
    }

    views[i] = attachment.ImageView;
  }

  vk::FramebufferCreateInfo createInfo =
  {
    {},
    renderPass,
    static_cast<uint32_t>(views.size()),
    views.data(),
    width,
    height,
    layers
  };

  auto result = device.createFramebuffer(&createInfo,
                                         nullptr,
                                         &currentFrameBuffer);

  assert(result == vk::Result::eSuccess);

  return currentFrameBuffer;
}

void lpe::render::RenderPass::Begin(vk::CommandBuffer cmdBuffer,
                                    vk::Rect2D renderArea,
                                    std::vector<vk::ClearValue> clearValues,
                                    vk::SubpassContents contents)
{
  assert((state == RenderPassState::Created || state == RenderPassState::Ended) && currentFrameBuffer && renderPass);

  vk::RenderPassBeginInfo beginInfo =
  {
    renderPass,
    currentFrameBuffer,
    renderArea,
    static_cast<uint32_t>(clearValues.size()),
    clearValues.data()
  };

  cmdBuffer.beginRenderPass(&beginInfo,
                            contents);

  currentCmdBuffer = cmdBuffer;

  state = RenderPassState::Recording;
}

void lpe::render::RenderPass::NextSubpass(vk::SubpassContents contents)
{
  assert(state == RenderPassState::Recording && currentFrameBuffer && renderPass && currentCmdBuffer);

  currentCmdBuffer.nextSubpass(contents);
}

void lpe::render::RenderPass::End(vk::Device device)
{
  assert(state == RenderPassState::Recording && currentCmdBuffer && currentFrameBuffer);

  currentCmdBuffer.endRenderPass();

  device.destroyFramebuffer(currentFrameBuffer,
                            nullptr);
  currentFrameBuffer = nullptr;
  currentCmdBuffer = nullptr;

  state = RenderPassState::Ended;
}

void lpe::render::RenderPass::Destroy(vk::Device device)
{
  assert(state == RenderPassState::Ended && renderPass);

  if (currentFrameBuffer)
  {
    device.destroyFramebuffer(currentFrameBuffer,
                              nullptr);
    currentFrameBuffer = nullptr;
  }

  device.destroyRenderPass(renderPass,
                           nullptr);
  renderPass = nullptr;
}
