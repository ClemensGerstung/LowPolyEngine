#include "../include/Device.h"
#include "../include/Instance.h"
#include <set>

lpe::Device::Device(const Device& device)
{
  instance.reset(device.instance.get());
  this->physicalDevice = device.physicalDevice;
  this->device = device.device;
  this->surface = device.surface;
  this->graphicsQueue = device.graphicsQueue;
  this->presentQueue = device.presentQueue;
  this->indices = device.indices;
  this->pipelineCache = device.pipelineCache;
}

lpe::Device::Device(Device&& device) noexcept
{
  instance = std::move(device.instance);
  this->physicalDevice = device.physicalDevice;
  this->device = device.device;
  this->surface = device.surface;
  this->graphicsQueue = device.graphicsQueue;
  this->presentQueue = device.presentQueue;
  this->indices = device.indices;
  this->pipelineCache = device.pipelineCache;
}

lpe::Device& lpe::Device::operator=(const Device& device)
{
  instance.reset(device.instance.get());
  this->physicalDevice = device.physicalDevice;
  this->device = device.device;
  this->surface = device.surface;
  this->graphicsQueue = device.graphicsQueue;
  this->presentQueue = device.presentQueue;
  this->indices = device.indices;
  this->pipelineCache = device.pipelineCache;
  return *this;
}

lpe::Device& lpe::Device::operator=(Device&& device) noexcept
{
  instance = std::move(device.instance);
  this->physicalDevice = device.physicalDevice;
  this->graphicsQueue = device.graphicsQueue;
  this->presentQueue = device.presentQueue;
  this->indices = device.indices;
  this->device = device.device;
  this->surface = device.surface;
  this->pipelineCache = device.pipelineCache;
  return *this;
}

lpe::Device::Device(vk::Instance* instance,
                    vk::PhysicalDevice physicalDevice,
                    const vk::SurfaceKHR& surface)
  : physicalDevice(physicalDevice),
    surface(surface)
{
  this->instance.reset(instance);
  this->indices = Instance::FindQueueFamilies(this->physicalDevice,
                                              this->surface);

  float queuePriority = 1.0f;

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> queueFamiliyIndices = { indices.graphicsFamily, indices.presentFamily };

  for (auto queueFamily : queueFamiliyIndices)
  {
    queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority);
  }

  vk::PhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  vk::DeviceCreateInfo createInfo = {};
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.enabledLayerCount = static_cast<uint32_t>(helper::ValidationLayer.size());
  createInfo.ppEnabledLayerNames = helper::ValidationLayer.data();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(helper::DeviceExtensions.size());
  createInfo.ppEnabledExtensionNames = helper::DeviceExtensions.data();
  createInfo.pEnabledFeatures = &deviceFeatures;

  device = this->physicalDevice.createDevice(createInfo,
                                             nullptr);

  presentQueue = device.getQueue(indices.presentFamily,
                                 0);
  graphicsQueue = device.getQueue(indices.graphicsFamily,
                                  0);

  vk::PipelineCacheCreateInfo cacheCreateInfo = {};
  auto result = device.createPipelineCache(&cacheCreateInfo,
                                           nullptr,
                                           &pipelineCache);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create PipelineCache");
}

lpe::Device::~Device()
{
  if (instance)
  {
    if (surface)
    {
      instance->destroySurfaceKHR(surface);
    }

    if (pipelineCache)
    {
      device.destroyPipelineCache(pipelineCache);
    }

    if (device)
    {
      device.destroy(nullptr);
    }

    instance.release();
  }
}

vk::Format lpe::Device::FindDepthFormat() const
{
  vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

  for (vk::Format format : { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint })
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

  throw std::runtime_error("Failed to find supported format!");
}

lpe::SwapChain lpe::Device::CreateSwapChain(uint32_t width,
                                            uint32_t height)
{
  return { physicalDevice, std::make_unique<vk::Device>(device), surface, indices, width, height };
}

lpe::Commands lpe::Device::CreateCommands()
{
  return { physicalDevice, &device, &graphicsQueue, indices.graphicsFamily };
}

lpe::Renderer lpe::Device::CreateRenderer(lpe::BufferMemory* bufferMemory,
                                          uint32_t bufferId)
{
  return { &device, physicalDevice, bufferMemory, bufferId };
}

lpe::RenderPass lpe::Device::CreateRenderPass(vk::Format swapChainImageFormat)
{
  return { std::unique_ptr<vk::Device>(&device), swapChainImageFormat, FindDepthFormat() };
}

vk::SubmitInfo lpe::Device::PrepareFrame(const SwapChain& swapChain,
                                         uint32_t* imageIndex)
{
  if (!imageAvailableSemaphore)
  {
    vk::SemaphoreCreateInfo semaphoreInfo = {};

    auto result = device.createSemaphore(&semaphoreInfo,
                                         nullptr,
                                         &imageAvailableSemaphore);
    helper::ThrowIfNotSuccess(result,
                              "Failed to create imageAvailableSemaphore!");
  }

  if (!renderAvailableSemaphore)
  {
    vk::SemaphoreCreateInfo semaphoreInfo = {};

    auto result = device.createSemaphore(&semaphoreInfo,
                                         nullptr,
                                         &renderAvailableSemaphore);
    helper::ThrowIfNotSuccess(result,
                              "Failed to create renderAvailableSemaphore!");
  }

  auto result = device.acquireNextImageKHR(swapChain.GetSwapchain(),
                                           std::numeric_limits<uint64_t>::max(),
                                           imageAvailableSemaphore,
                                           {},
                                           imageIndex);

  if (result == vk::Result::eErrorOutOfDateKHR)
  {
    // TOOD: recreate swapchain
    return {};
  }
  else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
  {
    throw std::runtime_error("Failed to acquire swap chain image! (" + vk::to_string(result) + ")");
  }

  vk::SubmitInfo submitInfo = { 1, &imageAvailableSemaphore, waitFlags, 0, nullptr, 1, &renderAvailableSemaphore };

  return submitInfo;
}

void lpe::Device::SubmitQueue(uint32_t submitCount,
                              const vk::SubmitInfo* infos)
{
  auto result = graphicsQueue.submit(submitCount,
                                     infos,
                                     nullptr);
  helper::ThrowIfNotSuccess(result,
                            "Failed to submit draw command buffer!");
}

void lpe::Device::SubmitFrame(const std::vector<vk::SwapchainKHR>& swapChains,
                              uint32_t* imageIndex)
{
  vk::Semaphore signalSemaphores[] = { renderAvailableSemaphore };

  vk::PresentInfoKHR presentInfo = { 1, signalSemaphores, (uint32_t)swapChains.size(), swapChains.data(), imageIndex };

  auto result = presentQueue.presentKHR(&presentInfo);
  //auto result = vk::Result::eSuccess;

  if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
  {
    // TODO: recreate swapchain
  }
  else if (result != vk::Result::eSuccess)
  {
    throw std::runtime_error("Failed to present swap chain image! (Result: " + vk::to_string(result) + ")");
  }

  presentQueue.waitIdle();
}

std::vector<lpe::Pipeline> lpe::Device::CreatePipelines(const SwapChain& swapChain,
                                                        RenderPass& renderPass,
                                                        BufferMemory* buffer,
                                                        uint32_t bufferId)
{
  std::vector<lpe::Pipeline> pipes = {};

  Pipeline::CreateInfo::ShaderInfo vertex = {
    "shaders/base.vert.spv",
    {},
    vk::ShaderStageFlagBits::eVertex,
    "main"
  };
  Pipeline::CreateInfo::ShaderInfo fragement = {
    "shaders/base.frag.spv",
    {},
    vk::ShaderStageFlagBits::eFragment,
    "main"
  };

  Pipeline::CreateInfo info = {};
  info.type = vk::PipelineBindPoint::eGraphics;
  info.allowTransperency = false;
  info.prio = 0;
  info.attributeDescriptions = lpe::Vertex::GetAttributeDescriptions();
  info.bindingDescriptions = lpe::Vertex::GetBindingDescription();
  info.pushConstantRanges = {};
  info.shaders.emplace_back(vertex);
  info.shaders.emplace_back(fragement);
  info.renderPass = renderPass;
  info.swapChainExtent = swapChain.GetExtent();
  info.descriptors = {
    buffer->SetupDescriptor(bufferId,
                            buffer->GetSize(bufferId,
                                            BufferMemory::Type::UBO),
                            buffer->GetOffset(bufferId,
                                              BufferMemory::Type::UBO))
  };

  pipes.emplace_back(physicalDevice,
                     &device,
                     pipelineCache,
                     info);

  info.prio = 1;
  pipes.emplace_back(physicalDevice,
                     &device,
                     pipelineCache,
                     info);

  return pipes;
}

lpe::Device::operator bool() const
{
  return static_cast<bool>(device) && static_cast<bool>(surface);
}

bool lpe::Device::operator!() const
{
  return !(static_cast<bool>(device) && static_cast<bool>(surface));
}

lpe::BufferMemory lpe::Device::CreateBuffer(uint32_t vertexCount,
                                            uint32_t indexCount,
                                            uint32_t instanceCount,
                                            uint32_t indirectCommandsCount)
{
  lpe::BufferMemoryCreateInfo<1> createInfo = {};
  createInfo.ids = { 1 };
  createInfo.usages = {
    vk::BufferUsageFlagBits::eUniformBuffer |
    vk::BufferUsageFlagBits::eIndexBuffer |
    vk::BufferUsageFlagBits::eVertexBuffer |
    vk::BufferUsageFlagBits::eIndirectBuffer
  };
  createInfo.propertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
  std::map<uint32_t, vk::DeviceSize> offsets = {
    { BufferMemory::Type::Vertex, vertexCount * sizeof(lpe::Vertex) },
    { BufferMemory::Type::Index, indexCount * sizeof(uint32_t) },
    { BufferMemory::Type::UBO, sizeof(UniformBufferObject) },
    { BufferMemory::Type::Instance, instanceCount * sizeof(InstanceData) },
    { BufferMemory::Type::Indirect, indirectCommandsCount * sizeof(vk::DrawIndexedIndirectCommand) },
  };
  createInfo.offsets = { offsets };

  lpe::BufferMemory memory = { &device, physicalDevice, createInfo };

  return memory;
}
