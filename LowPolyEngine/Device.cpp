#include "Device.h"
#include "Instance.h"

lpe::Device::Device(const Device& device)
{
  instance.reset(device.instance.get());
  this->physicalDevice = device.physicalDevice;
  this->device = device.device;
  this->surface = device.surface;
  this->graphicsQueue = device.graphicsQueue;
  this->presentQueue = device.presentQueue;
  this->indices = device.indices;
}

lpe::Device::Device(Device&& device)
{
  instance.reset(device.instance.get());
  device.instance.release();
  this->physicalDevice = device.physicalDevice;
  this->device = device.device;
  this->surface = device.surface;
  this->graphicsQueue = device.graphicsQueue;
  this->presentQueue = device.presentQueue;
  this->indices = device.indices;
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
  return *this;
}

lpe::Device& lpe::Device::operator=(Device&& device)
{
  instance.reset(device.instance.get());
  device.instance.release();
  this->physicalDevice = device.physicalDevice;
  this->graphicsQueue = device.graphicsQueue;
  this->presentQueue = device.presentQueue;
  this->indices = device.indices;
  this->device = device.device;
  this->surface = device.surface;
  return *this;
}

lpe::Device::Device(vk::Instance* instance, vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR& surface)
  : physicalDevice(physicalDevice),
    surface(surface)
{
  this->instance.reset(instance);
  this->indices = Instance::FindQueueFamilies(this->physicalDevice, this->surface);

  float queuePriority = 1.0f;

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

  for (auto queueFamily : { indices.graphicsFamily, indices.presentFamily })
  {
    queueCreateInfos.push_back({ {}, queueFamily, 1, &queuePriority });
  }

  vk::DeviceCreateInfo createInfo =
  {
    {},
    (uint32_t)queueCreateInfos.size(),
    queueCreateInfos.data(),
#if defined(ENABLE_VALIDATION_LAYER) && ENABLE_VALIDATION_LAYER
    (uint32_t)helper::ValidationLayer.size(),
    helper::ValidationLayer.data(),
#else
    0,
    nullptr,
#endif
    (uint32_t)helper::DeviceExtensions.size(),
    helper::DeviceExtensions.data(),
    {}
  };

  device = this->physicalDevice.createDevice(createInfo, nullptr);

  presentQueue = device.getQueue(indices.presentFamily, 0);
  graphicsQueue = device.getQueue(indices.graphicsFamily, 0);

  vk::PipelineCacheCreateInfo cacheCreateInfo = {};
  auto result = device.createPipelineCache(&cacheCreateInfo, nullptr, &pipelineCache);
  helper::ThrowIfNotSuccess(result, "Couldn't create pipelinecache");
}

lpe::Device::~Device()
{
  if (instance)
  {
    if (surface)
    {
      instance->destroySurfaceKHR(surface);
    }

    if(pipelineCache)
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

lpe::SwapChain lpe::Device::CreateSwapChain(uint32_t width, uint32_t height)
{
  return {physicalDevice, &device, surface, indices, width, height};
}

lpe::Commands lpe::Device::CreateCommands()
{
  return {physicalDevice, &device, &graphicsQueue, indices.graphicsFamily};
}

lpe::ModelsRenderer lpe::Device::CreateModelsRenderer(Commands* commands)
{
  return { commands };
}

lpe::UniformBuffer lpe::Device::CreateUniformBuffer(const ModelsRenderer& modelsRenderer, const Camera& camera)
{
  return { physicalDevice, &device, modelsRenderer, camera };
}

lpe::Pipeline lpe::Device::CreatePipeline(const SwapChain& swapChain, UniformBuffer* ubo)
{
  return {physicalDevice, &device, &pipelineCache, swapChain.GetImageFormat(), swapChain.GetExtent(), ubo};
}

lpe::Device::operator bool() const
{
  return static_cast<bool>(device) && static_cast<bool>(surface);
}

bool lpe::Device::operator!() const
{
  return !(static_cast<bool>(device) && static_cast<bool>(surface));
}
