#include "../include/Pipeline.h"
#include "../include/Vertex.h"

void lpe::Pipeline::CreateDescriptorPool()
{
  std::vector<vk::DescriptorPoolSize> poolSizes =
  {
    {vk::DescriptorType::eUniformBuffer, 1}
  };

  vk::DescriptorPoolCreateInfo poolInfo = { {}, 2, (uint32_t)poolSizes.size(), poolSizes.data() };

  auto result = device->createDescriptorPool(&poolInfo, nullptr, &descriptorPool);
  helper::ThrowIfNotSuccess(result, "failed to create descriptor pool!");
}

void lpe::Pipeline::CreateDescriptorSetLayout()
{
  std::vector<vk::DescriptorSetLayoutBinding> bindings = 
  {
    { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex }
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

void lpe::Pipeline::CreatePipeline(vk::Extent2D swapChainExtent, vk::RenderPass renderPass)
{
  auto vertexShaderCode = lpe::helper::ReadSPIRVFile("shaders/base.vert.spv");
  auto fragmentShaderCode = lpe::helper::ReadSPIRVFile("shaders/base.frag.spv");

  auto vertexShaderModule = CreateShaderModule(vertexShaderCode);
  auto fragmentShaderModule = CreateShaderModule(fragmentShaderCode);

  vk::PipelineShaderStageCreateInfo vertexShaderStageInfo = { {}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main" };
  vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo = { {}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main" };

  auto bindingDescriptions = Vertex::GetBindingDescription();
  auto attributeDescriptions = Vertex::GetAttributeDescriptions();

  std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = { vertexShaderStageInfo, fragmentShaderStageInfo };

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo = { {}, (uint32_t)bindingDescriptions.size(), bindingDescriptions.data(), (uint32_t)attributeDescriptions.size(), attributeDescriptions.data() };

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {}, vk::PrimitiveTopology::eTriangleList, VK_FALSE };

  vk::Viewport viewport = { 0.f, 0.f, (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };

  vk::Rect2D scissor = { { 0, 0 }, swapChainExtent };

  vk::PipelineViewportStateCreateInfo viewportState = { {}, 1, &viewport, 1, &scissor };

  vk::PipelineRasterizationStateCreateInfo rasterizer = { {}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, VK_FALSE, 0, 0, 0, 1 };

  //vk::PipelineRasterizationStateCreateInfo rasterizer = {};
  //rasterizer.polygonMode = vk::PolygonMode::eFill;
  //rasterizer.depthClampEnable = VK_FALSE;
  //rasterizer.cullMode = vk::CullModeFlagBits::eBack;
  //rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
  //rasterizer.lineWidth = 1.0f;

  vk::PipelineMultisampleStateCreateInfo multisampling = { {}, vk::SampleCountFlagBits::e1, VK_FALSE };

  vk::PipelineDepthStencilStateCreateInfo depthStencil = { {}, VK_TRUE, VK_TRUE, vk::CompareOp::eLessOrEqual, VK_FALSE, VK_FALSE };
  depthStencil.front = depthStencil.back;
  depthStencil.back.compareOp = vk::CompareOp::eAlways;
  depthStencil.maxDepthBounds = 1.0;

  vk::PipelineColorBlendAttachmentState colorBlendAttachment = { VK_FALSE };
  colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

  std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dynamicState = { {}, (uint32_t)dynamicStates.size(), dynamicStates.data() };

  vk::PipelineColorBlendStateCreateInfo colorBlending = { {}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment };
  colorBlending.blendConstants[0] = 0;
  colorBlending.blendConstants[1] = 0;
  colorBlending.blendConstants[2] = 0;
  colorBlending.blendConstants[3] = 0;

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = { {}, 1, &descriptorSetLayout };

  auto result = device->createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout);
  helper::ThrowIfNotSuccess(result, "failed to create pipeline layout!");

  vk::GraphicsPipelineCreateInfo pipelineInfo = { {}, (uint32_t)shaderStages.size(), shaderStages.data(), &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, &depthStencil, &colorBlending, &dynamicState, pipelineLayout, renderPass };

  pipeline = device->createGraphicsPipeline(cache, pipelineInfo);

  device->destroyShaderModule(vertexShaderModule);
  device->destroyShaderModule(fragmentShaderModule);
}

void lpe::Pipeline::UpdateDescriptorSets()
{
  auto descriptors = ubo->GetDescriptors();

  if (!descriptorSet)
  {
    std::array<vk::DescriptorSetLayout, 1> layouts = { descriptorSetLayout };

    vk::DescriptorSetAllocateInfo allocInfo = { descriptorPool, (uint32_t)layouts.size(), layouts.data() };

    auto result = device->allocateDescriptorSets(&allocInfo, &descriptorSet);
    helper::ThrowIfNotSuccess(result, "failed to allocate descriptor set!");
  }

  vk::WriteDescriptorSet uboWriteDescriptorSet = { descriptorSet };
  uboWriteDescriptorSet.dstBinding = 0;
  uboWriteDescriptorSet.descriptorCount = 1;
  uboWriteDescriptorSet.descriptorType = vk::DescriptorType::eUniformBuffer;
  uboWriteDescriptorSet.pBufferInfo = &descriptors[0];

  std::vector<vk::WriteDescriptorSet> descriptorWrites = { uboWriteDescriptorSet };
 
  device->updateDescriptorSets((uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void lpe::Pipeline::Copy(const Pipeline& other)
{
  this->physicalDevice = other.physicalDevice;
  this->device.reset(other.device.get());
  this->ubo.reset(other.ubo.get());

  this->cache = other.cache;
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
                        vk::PipelineCache cache,
                        vk::RenderPass renderPass,
                        vk::Extent2D swapChainExtent,
                        lpe::UniformBuffer* uniformBuffer)
  : physicalDevice(physicalDevice),
    cache(cache)
{
  this->device.reset(device);
  this->ubo.reset(uniformBuffer);

  CreateDescriptorSetLayout();

  CreatePipeline(swapChainExtent, renderPass);

  CreateDescriptorPool();

  UpdateDescriptorSets();
}

lpe::Pipeline::~Pipeline()
{
  if(ubo)
  {
    ubo.release();
  }

  if(device)
  {
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
