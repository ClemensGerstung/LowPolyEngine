#include "../include/RenderPass.h"

void lpe::RenderPass::CreateRenderPass(vk::Format swapChainImageFormat, vk::Format depthFormat, bool useDepth)
{
  vk::AttachmentDescription colorAttachment =
  {
    {},
    swapChainImageFormat,
    vk::SampleCountFlagBits::e1,
    vk::AttachmentLoadOp::eClear,
    vk::AttachmentStoreOp::eStore,
    vk::AttachmentLoadOp::eDontCare,
    vk::AttachmentStoreOp::eDontCare,
    vk::ImageLayout::eUndefined,
    vk::ImageLayout::ePresentSrcKHR
  };
  vk::AttachmentDescription depthAttachment =
  {
    {},
    depthFormat,
    vk::SampleCountFlagBits::e1,
    vk::AttachmentLoadOp::eClear,
    vk::AttachmentStoreOp::eStore,
    vk::AttachmentLoadOp::eClear,
    vk::AttachmentStoreOp::eDontCare,
    vk::ImageLayout::eUndefined,
    vk::ImageLayout::eDepthStencilAttachmentOptimal
  };

  vk::AttachmentReference colorAttachmentRef =
  {
    0,
    vk::ImageLayout::eColorAttachmentOptimal
  };

  vk::AttachmentReference depthAttachmentRef =
  {
    1,
    vk::ImageLayout::eDepthStencilAttachmentOptimal
  };

  vk::SubpassDescription subpass = { {}, vk::PipelineBindPoint::eGraphics };
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  if (useDepth)
  {
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
  }

  vk::SubpassDependency dependency =
  {
    VK_SUBPASS_EXTERNAL,
    0,
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    {},
    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
  };

  std::vector<vk::AttachmentDescription> attachments = {};
  attachments.push_back(colorAttachment);
  if(useDepth)
  {
    attachments.push_back(depthAttachment);
  }

  vk::RenderPassCreateInfo renderPassInfo =
  {
    {},
    (uint32_t)attachments.size(),
    attachments.data(),
    1,
    &subpass,
    1,
    &dependency
  };

  auto result = device->createRenderPass(&renderPassInfo, nullptr, &renderPass);
  helper::ThrowIfNotSuccess(result, "failed to create render pass!");
}

lpe::RenderPass::RenderPass(const RenderPass& other)
{
  device.reset(other.device.get());
  renderPass = other.renderPass;
}

lpe::RenderPass::RenderPass(RenderPass&& other) noexcept
{
  device = std::move(other.device);
  renderPass = other.renderPass;
}

lpe::RenderPass& lpe::RenderPass::operator=(const RenderPass& other)
{
  device.reset(other.device.get());
  renderPass = other.renderPass;

  return *this;
}

lpe::RenderPass& lpe::RenderPass::operator=(RenderPass&& other) noexcept
{
  device = std::move(other.device);
  renderPass = other.renderPass;

  return *this;
}

lpe::RenderPass::RenderPass(std::unique_ptr<vk::Device> device,
                            vk::Format swapChainImageFormat,
                            vk::Format depthFormat)
{
  this->device.swap(device);

  CreateRenderPass(swapChainImageFormat, depthFormat, true);
}

lpe::RenderPass::RenderPass(std::unique_ptr<vk::Device> device,
                            vk::Format swapChainImageFormat)
{
  this->device.swap(device);

  CreateRenderPass(swapChainImageFormat, vk::Format::eUndefined, false);
}

lpe::RenderPass::~RenderPass()
{
  if(device)
  {
    if(renderPass)
    {
      device->destroyRenderPass(renderPass);
    }
    
    device.release();
  }
}

vk::RenderPass lpe::RenderPass::GetRenderPass() const
{
  return renderPass;
}

lpe::RenderPass::operator vk::RenderPass() const
{
  return renderPass;
}
