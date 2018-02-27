#include "../include/Pipeline.h"
#include "../include/Vertex.h"

void lpe::Pipeline::CreateDescriptorPool()
{
  std::vector<vk::DescriptorPoolSize> poolSizes =
  {
    { vk::DescriptorType::eUniformBuffer, 1 }
  };

  vk::DescriptorPoolCreateInfo poolInfo = {
    vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    2,
    (uint32_t)poolSizes.size(),
    poolSizes.data()
  };

  auto result = device->createDescriptorPool(&poolInfo,
                                             nullptr,
                                             &descriptorPool);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create DescriptorPool!");
}

void lpe::Pipeline::CreateDescriptorSetLayout()
{
  std::vector<vk::DescriptorSetLayoutBinding> bindings =
  {
    { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex }
  };

  vk::DescriptorSetLayoutCreateInfo layoutInfo = { {}, (uint32_t)bindings.size(), bindings.data() };

  auto result = device->createDescriptorSetLayout(&layoutInfo,
                                                  nullptr,
                                                  &descriptorSetLayout);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create DescriptorSetLayout!");
}

lpe::Pipeline::CreateInfo::ShaderInfo::ShaderInfo(const std::string& fileName,
                                                  const std::vector<char>& compiledCode,
                                                  vk::ShaderStageFlagBits type,
                                                  const std::string& entryPoint)
  : fileName(fileName),
    compiledCode(compiledCode),
    type(type),
    entryPoint(entryPoint)
{
}

vk::ShaderModule lpe::Pipeline::CreateShaderModule(const std::vector<char>& code)
{
  vk::ShaderModuleCreateInfo createInfo = { {}, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };

  vk::ShaderModule shaderModule;

  auto result = device->createShaderModule(&createInfo,
                                           nullptr,
                                           &shaderModule);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create ShaderModule!");

  return shaderModule;
}

vk::Pipeline lpe::Pipeline::GetPipeline() const
{
  return pipeline;
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

bool lpe::Pipeline::AllowsTransparency() const
{
  return transparent;
}

uint32_t lpe::Pipeline::GetPrio() const
{
  return priority;
}

vk::PipelineBindPoint lpe::Pipeline::GetType() const
{
  return type;
}

void lpe::Pipeline::CreatePipeline(vk::Extent2D swapChainExtent,
                                   vk::RenderPass renderPass,
                                   const CreateInfo& info)
{
  std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {};
  std::vector<vk::ShaderModule> modules = {};

  for (const auto& shader : info.shaders)
  {
    if (shader.fileName.empty() && shader.compiledCode.empty())
    {
      throw std::runtime_error("Got Shader with no compiled SPIR-V code or filename");
    }

    std::vector<char> shaderCode;

    if (!shader.fileName.empty() && shader.compiledCode.empty())
    {
      shaderCode = lpe::helper::ReadSPIRVFile(shader.fileName);
    }
    else if (shader.fileName.empty() && !shader.compiledCode.empty())
    {
      shaderCode = shader.compiledCode;
    }

    vk::ShaderModule shaderModule = CreateShaderModule(shaderCode);
    vk::PipelineShaderStageCreateInfo shaderStageInfo = { {}, shader.type, shaderModule, shader.entryPoint.c_str() };

    modules.push_back(shaderModule);
    shaderStages.push_back(shaderStageInfo);
  }

  auto bindingDescriptions = info.bindingDescriptions;
  auto attributeDescriptions = info.attributeDescriptions;

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {
    {},
    (uint32_t)bindingDescriptions.size(),
    bindingDescriptions.data(),
    (uint32_t)attributeDescriptions.size(),
    attributeDescriptions.data()
  };

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {}, vk::PrimitiveTopology::eTriangleList, VK_FALSE };

  vk::Viewport viewport = { 0.f, 0.f, (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };

  vk::Rect2D scissor = { { 0, 0 }, swapChainExtent };

  vk::PipelineViewportStateCreateInfo viewportState = { {}, 1, &viewport, 1, &scissor };

  vk::PipelineRasterizationStateCreateInfo rasterizer = {
    {},
    VK_FALSE,
    VK_FALSE,
    vk::PolygonMode::eFill,
    vk::CullModeFlagBits::eBack,
    vk::FrontFace::eCounterClockwise,
    VK_FALSE,
    0,
    0,
    0,
    1
  };

  //vk::PipelineRasterizationStateCreateInfo rasterizer = {};
  //rasterizer.polygonMode = vk::PolygonMode::eFill;
  //rasterizer.depthClampEnable = VK_FALSE;
  //rasterizer.cullMode = vk::CullModeFlagBits::eBack;
  //rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
  //rasterizer.lineWidth = 1.0f;

  vk::PipelineMultisampleStateCreateInfo multisampling = { {}, vk::SampleCountFlagBits::e1, VK_FALSE };

  vk::PipelineDepthStencilStateCreateInfo depthStencil = {
    {},
    VK_TRUE,
    VK_TRUE,
    vk::CompareOp::eLessOrEqual,
    VK_FALSE,
    VK_FALSE
  };
  depthStencil.front = depthStencil.back;
  depthStencil.back.compareOp = vk::CompareOp::eAlways;
  depthStencil.maxDepthBounds = 1.0;

  vk::PipelineColorBlendAttachmentState colorBlendAttachment = { VK_FALSE };

  if (info.allowTransperency)
  {
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
  }

  colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
    vk::ColorComponentFlagBits::eG |
    vk::ColorComponentFlagBits::eB |
    vk::ColorComponentFlagBits::eA;

  std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
  vk::PipelineDynamicStateCreateInfo dynamicState = { {}, (uint32_t)dynamicStates.size(), dynamicStates.data() };

  vk::PipelineColorBlendStateCreateInfo colorBlending = { {}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment };
  colorBlending.blendConstants[0] = 0;
  colorBlending.blendConstants[1] = 0;
  colorBlending.blendConstants[2] = 0;
  colorBlending.blendConstants[3] = 0;

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = { {}, 1, &descriptorSetLayout };
  if (!info.pushConstantRanges.empty())
  {
    pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)info.pushConstantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges = info.pushConstantRanges.data();
  }

  auto result = device->createPipelineLayout(&pipelineLayoutInfo,
                                             nullptr,
                                             &pipelineLayout);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create PipelineLayout!");

  vk::GraphicsPipelineCreateInfo pipelineInfo =
  {
    {},
    (uint32_t)shaderStages.size(),
    shaderStages.data(),
    &vertexInputInfo,
    &inputAssembly,
    nullptr,
    &viewportState,
    &rasterizer,
    &multisampling,
    &depthStencil,
    &colorBlending,
    &dynamicState,
    pipelineLayout,
    renderPass
  };

  pipeline = device->createGraphicsPipeline(cache,
                                            pipelineInfo);

  for (const auto& module : modules)
  {
    device->destroyShaderModule(module);
  }
}

void lpe::Pipeline::UpdateDescriptorSets(std::vector<vk::DescriptorBufferInfo> descriptors)
{
  if (!descriptorSet)
  {
    std::array<vk::DescriptorSetLayout, 1> layouts = { descriptorSetLayout };

    vk::DescriptorSetAllocateInfo allocInfo = { descriptorPool, (uint32_t)layouts.size(), layouts.data() };

    auto result = device->allocateDescriptorSets(&allocInfo,
                                                 &descriptorSet);
    helper::ThrowIfNotSuccess(result,
                              "Failed to allocate DescriptorSets!");
  }

  std::vector<vk::WriteDescriptorSet> descriptorSets = {};
  descriptorSets.emplace_back(descriptorSet,
                              0,
                              0,
                              1,
                              vk::DescriptorType::eUniformBuffer,
                              nullptr,
                              &descriptors[0]);

  device->updateDescriptorSets((uint32_t)descriptorSets.size(),
                               descriptorSets.data(),
                               0,
                               nullptr);
}

lpe::Pipeline::Pipeline(const Pipeline& other)
{
  this->physicalDevice = other.physicalDevice;
  this->device.reset(other.device.get());

  this->priority = other.priority;
  this->transparent = other.transparent;
  this->type = other.type;
  this->cache = other.cache;
  this->descriptorSetLayout = other.descriptorSetLayout;
  this->pipelineLayout = other.pipelineLayout;
  this->pipeline = other.pipeline;
  this->descriptorPool = other.descriptorPool;
  this->descriptorSet = other.descriptorSet;
}

lpe::Pipeline::Pipeline(Pipeline&& other) noexcept
{
  this->physicalDevice = other.physicalDevice;
  this->device = std::move(other.device);

  this->priority = other.priority;
  this->transparent = other.transparent;
  this->type = other.type;
  this->cache = other.cache;
  this->descriptorSetLayout = other.descriptorSetLayout;
  this->pipelineLayout = other.pipelineLayout;
  this->pipeline = other.pipeline;
  this->descriptorPool = other.descriptorPool;
  this->descriptorSet = other.descriptorSet;
}

lpe::Pipeline& lpe::Pipeline::operator=(const Pipeline& other)
{
  this->physicalDevice = other.physicalDevice;
  this->device.reset(other.device.get());

  this->priority = other.priority;
  this->transparent = other.transparent;
  this->type = other.type;
  this->cache = other.cache;
  this->descriptorSetLayout = other.descriptorSetLayout;
  this->pipelineLayout = other.pipelineLayout;
  this->pipeline = other.pipeline;
  this->descriptorPool = other.descriptorPool;
  this->descriptorSet = other.descriptorSet;

  return *this;
}

lpe::Pipeline& lpe::Pipeline::operator=(Pipeline&& other) noexcept
{
  this->physicalDevice = other.physicalDevice;
  this->device = std::move(other.device);

  this->priority = other.priority;
  this->transparent = other.transparent;
  this->type = other.type;
  this->cache = other.cache;
  this->descriptorSetLayout = other.descriptorSetLayout;
  this->pipelineLayout = other.pipelineLayout;
  this->pipeline = other.pipeline;
  this->descriptorPool = other.descriptorPool;
  this->descriptorSet = other.descriptorSet;

  return *this;
}

lpe::Pipeline::Pipeline(vk::PhysicalDevice physicalDevice,
                        vk::Device* device,
                        vk::PipelineCache cache,
                        CreateInfo createInfo)
  : physicalDevice(physicalDevice),
    cache(cache),
    transparent(createInfo.allowTransperency),
    priority(createInfo.prio),
    type(createInfo.type)
{
  this->device.reset(device);

  CreateDescriptorSetLayout();

  CreatePipeline(createInfo.swapChainExtent,
                 createInfo.renderPass,
                 createInfo);

  CreateDescriptorPool();

  UpdateDescriptorSets(createInfo.descriptors);
}

lpe::Pipeline::~Pipeline()
{
  if (device)
  {
    if (descriptorSet)
    {
      auto result = device->freeDescriptorSets(descriptorPool,
                                               1,
                                               &descriptorSet);
    }

    if (descriptorSetLayout)
    {
      device->destroyDescriptorSetLayout(descriptorSetLayout);
    }

    if (pipelineLayout)
    {
      device->destroyPipelineLayout(pipelineLayout);
    }

    if (pipeline)
    {
      device->destroyPipeline(pipeline);
    }

    if (descriptorPool)
    {
      device->destroyDescriptorPool(descriptorPool);
    }

    device.release();
  }
}
