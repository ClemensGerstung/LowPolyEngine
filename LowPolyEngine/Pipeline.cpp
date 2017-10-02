#include "Pipeline.h"
#include "Vertex.h"

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

vk::RenderPass* lpe::Pipeline::GetRenderPassRef()
{
  return &renderPass;
}

vk::Pipeline* lpe::Pipeline::GetPipelineRef()
{
  return &pipeline;
}

vk::PipelineLayout lpe::Pipeline::GetPipelineLayout() const
{
  return pipelineLayout;
}

vk::DescriptorSet lpe::Pipeline::GetDescriptorSet() const
{
  return descriptorSet;
}

vk::DescriptorSet* lpe::Pipeline::GetDescriptorSetRef()
{
  return &descriptorSet;
}

vk::RenderPass lpe::Pipeline::GetRenderPass() const
{
  return renderPass;
}


void lpe::Pipeline::CreatePipeline(vk::Extent2D swapChainExtent)
{
  auto vertexShaderCode = lpe::helper::ReadSPIRVFile("shaders/base.vert.spv");
  auto fragmentShaderCode = lpe::helper::ReadSPIRVFile("shaders/base.frag.spv");

  auto vertexShaderModule = CreateShaderModule(vertexShaderCode);
  auto fragmentShaderModule = CreateShaderModule(fragmentShaderCode);

  vk::PipelineShaderStageCreateInfo vertexShaderStageInfo = { {}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main" };
  vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo = { {}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main" };

  auto bindingDescription = Vertex::getBindingDescription();
  auto attributeDescriptions = Vertex::getAttributeDescriptions();

  std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = { vertexShaderStageInfo, fragmentShaderStageInfo };

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo = { {}, 1, &bindingDescription, (uint32_t)attributeDescriptions.size(), attributeDescriptions.data() };

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {}, vk::PrimitiveTopology::eTriangleList, VK_FALSE };

  vk::Viewport viewport = { 0.f, 0.f, (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };

  vk::Rect2D scissor = { { 0, 0 }, swapChainExtent };

  vk::PipelineViewportStateCreateInfo viewportState = { {}, 1, &viewport, 1, &scissor };

  vk::PipelineRasterizationStateCreateInfo rasterizer = { {}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, VK_FALSE, 0, 0, 0, 1 };

  vk::PipelineMultisampleStateCreateInfo multisampling = { {}, vk::SampleCountFlagBits::e1, VK_FALSE };

  vk::PipelineDepthStencilStateCreateInfo depthStencil = { {}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess, VK_FALSE, VK_FALSE };

  vk::PipelineColorBlendAttachmentState colorBlendAttachment = { VK_FALSE };
  colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

  vk::PipelineColorBlendStateCreateInfo colorBlending = { {}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment };
  colorBlending.blendConstants[0] = 0;
  colorBlending.blendConstants[1] = 0;
  colorBlending.blendConstants[2] = 0;
  colorBlending.blendConstants[3] = 0;

    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{
            {},2,std::array<vk::DynamicState,2>{vk::DynamicState::eScissor,vk::DynamicState::eViewport}.data()
    };

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = { {}, 1, &descriptorSetLayout };

  auto result = device->createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout);
  helper::ThrowIfNotSuccess(result, "failed to create pipeline layout!");

    vk::GraphicsPipelineCreateInfo pipelineInfo = { {}, (uint32_t)shaderStages.size(), shaderStages.data(), &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, &depthStencil, &colorBlending, &pipelineDynamicStateCreateInfo, pipelineLayout, renderPass };

  pipeline = device->createGraphicsPipeline(*cache, pipelineInfo);

  device->destroyShaderModule(vertexShaderModule);
  device->destroyShaderModule(fragmentShaderModule);
}

void lpe::Pipeline::CreateDescriptorSet()
{
  auto descriptors = ubo->GetDescriptors();

  // TODO: not quite nice but works for now!
  if (std::any_of(descriptors.begin(), descriptors.end(), [](const vk::DescriptorBufferInfo* info) { return !info->buffer; }))
  {
    return;
  }

  std::array<vk::DescriptorSetLayout, 1> layouts = { descriptorSetLayout };

  vk::DescriptorSetAllocateInfo allocInfo = { descriptorPool, (uint32_t)layouts.size(), layouts.data() };

  auto result = device->allocateDescriptorSets(&allocInfo, &descriptorSet);
  helper::ThrowIfNotSuccess(result, "failed to allocate descriptor set!");


  vk::WriteDescriptorSet uboVSwds = { descriptorSet };
  uboVSwds.dstBinding = 0;
  uboVSwds.descriptorCount = 1;
  uboVSwds.descriptorType = vk::DescriptorType::eUniformBuffer;
  uboVSwds.pBufferInfo = descriptors[0];

  vk::WriteDescriptorSet dynamicwds = { descriptorSet };
  dynamicwds.dstBinding = 1;
  dynamicwds.descriptorCount = 1;
  dynamicwds.descriptorType = vk::DescriptorType::eUniformBufferDynamic;
  dynamicwds.pBufferInfo = descriptors[1];

  std::vector<vk::WriteDescriptorSet> descriptorWrites = { uboVSwds, dynamicwds };

  device->updateDescriptorSets((uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void lpe::Pipeline::Copy(const Pipeline& other)
{
  this->physicalDevice = other.physicalDevice;
  this->device.reset(other.device.get());
  this->cache.reset(other.cache.get());
  this->ubo.reset(other.ubo.get());

  this->renderPass = other.renderPass;
  this->descriptorSetLayout = other.descriptorSetLayout;
  this->pipelineLayout = other.pipelineLayout;
  this->pipeline = other.pipeline;
  this->descriptorPool = other.descriptorPool;
  this->descriptorSet = other.descriptorSet;
}

void lpe::Pipeline::Move(Pipeline& other)
{
  Copy(other);
  other.device.release();
  other.cache.release();
  other.ubo.release();
}

lpe::Pipeline::Pipeline(const Pipeline& other)
{
  Copy(other);
}

lpe::Pipeline::Pipeline(Pipeline&& other)
{
  Move(other);
}

lpe::Pipeline& lpe::Pipeline::operator=(const Pipeline& other)
{
  Copy(other);
  return *this;
}

lpe::Pipeline& lpe::Pipeline::operator=(Pipeline&& other)
{
  Move(other);
  return *this;
}

lpe::Pipeline::Pipeline(vk::PhysicalDevice physicalDevice,
                        vk::Device* device,
                        vk::PipelineCache* cache,
                        vk::Format swapChainImageFormat,
                        vk::Extent2D swapChainExtent,
                        lpe::UniformBuffer* uniformBuffer)
  : physicalDevice(physicalDevice)
{
  this->device.reset(device);
  this->cache.reset(cache);
  this->ubo.reset(uniformBuffer);

  CreateRenderPass(swapChainImageFormat);

  CreateDescriptorSetLayout();

  CreatePipeline(swapChainExtent);

  CreateDescriptorPool();

  CreateDescriptorSet();
}

lpe::Pipeline::~Pipeline()
{
  if(ubo)
  {
    ubo.release();
  }

  if(cache)
  {
    cache.release();
  }

  if(device)
  {
    if(renderPass)
    {
      device->destroyRenderPass(renderPass);
    }

    if(descriptorSetLayout)
    {
      device->destroyDescriptorSetLayout(descriptorSetLayout);
    }

    if(pipelineLayout)
    {
      device->destroyPipelineLayout(pipelineLayout);
    }

    if(pipeline)
    {
      device->destroyPipeline(pipeline);
    }

    if(descriptorPool)
    {
      device->destroyDescriptorPool(descriptorPool);
    }

    device.release();
  }
}
