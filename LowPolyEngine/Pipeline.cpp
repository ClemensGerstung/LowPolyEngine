#include "Pipeline.h"

void lpe::Pipeline::CreateRenderPass(vk::Format swapChainImageFormat)
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
    FindDepthFormat(),
    vk::SampleCountFlagBits::e1,
    vk::AttachmentLoadOp::eClear,
    vk::AttachmentStoreOp::eDontCare,
    vk::AttachmentLoadOp::eDontCare,
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

  vk::SubpassDescription subpass =
  {
    {},
    vk::PipelineBindPoint::eGraphics,
    0,
    nullptr,
    1,
    &colorAttachmentRef,
    nullptr,
    &depthAttachmentRef,
    0,
    nullptr
  };

  vk::SubpassDependency dependency =
  {
    VK_SUBPASS_EXTERNAL,
    0,
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    {},
    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
  };

  std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

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

void lpe::Pipeline::CreateDescriptorPool()
{
  std::vector<vk::DescriptorPoolSize> poolSizes =
  {
    {vk::DescriptorType::eUniformBuffer, 1},
    {vk::DescriptorType::eUniformBufferDynamic, 1},
    {vk::DescriptorType::eCombinedImageSampler, 1}
  };

  vk::DescriptorPoolCreateInfo poolInfo = { {}, 2, (uint32_t)poolSizes.size(), poolSizes.data() };

  auto result = device->createDescriptorPool(&poolInfo, nullptr, &descriptorPool);
  helper::ThrowIfNotSuccess(result, "failed to create descriptor pool!");
}

void lpe::Pipeline::CreateDescriptorSetLayout()
{
  std::vector<vk::DescriptorSetLayoutBinding> bindings = 
  {
    { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex },
    { 1, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eVertex },
    { 2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }
  };
  vk::DescriptorSetLayoutBinding uboLayoutBinding = { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex };
  vk::DescriptorSetLayoutBinding samplerLayoutBinding = { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment };


  vk::DescriptorSetLayoutCreateInfo layoutInfo = { {}, (uint32_t)bindings.size(), bindings.data() };

  auto result = device->createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout);
  helper::ThrowIfNotSuccess(result, "failed to create descriptor set layout!");
}

vk::ShaderModule lpe::Pipeline::CreateShaderModule(const std::vector<char>& code)
{
  vk::ShaderModuleCreateInfo createInfo = { {}, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };

  vk::ShaderModule shaderModule;

  auto result = device->createShaderModule(&createInfo, nullptr, &shaderModule);
  helper::ThrowIfNotSuccess(result, "failed to create shader module!");

  return shaderModule;
}


lpe::Pipeline::Pipeline(vk::PhysicalDevice physicalDevice,
                        vk::Device* device,
                        vk::Format swapChainImageFormat,
                        vk::Extent2D swapChainExtent)
  : physicalDevice(physicalDevice)
{
  this->device.reset(device);

  CreateRenderPass(swapChainImageFormat);

  CreateDescriptorPool();
}

vk::Format lpe::Pipeline::FindDepthFormat() const
{
  vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

  for (vk::Format format : {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint})
  {
    auto props = physicalDevice.getFormatProperties(format);

    if ((props.linearTilingFeatures & features) == features)
    {
      return format;
    }

    if ((props.optimalTilingFeatures & features) == features)
    {
      return format;
    }
  }

  throw std::runtime_error("failed to find supported format!");
}
