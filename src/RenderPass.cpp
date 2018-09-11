#include "RenderPass.h"

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
