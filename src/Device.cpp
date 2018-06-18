#include "lpe.h"

lpe::vulkan::Device::Device(const Device& other)
{
  instance.reset(other.instance.get());
  physicalDevice.reset(other.physicalDevice.get());
  device = other.device;
  surface = other.surface;
}

lpe::vulkan::Device::Device(Device&& other) noexcept
{
  instance = std::move(other.instance);
  physicalDevice = std::move(other.physicalDevice);
  device = other.device;
  surface = other.surface;
}

lpe::vulkan::Device& lpe::vulkan::Device::operator=(const Device& other)
{
  instance.reset(other.instance.get());
  physicalDevice.reset(other.physicalDevice.get());
  device = other.device;
  surface = other.surface;

  return *this;
}

lpe::vulkan::Device& lpe::vulkan::Device::operator=(Device&& other) noexcept
{
  instance = std::move(other.instance);
  physicalDevice = std::move(other.physicalDevice);
  device = other.device;
  surface = other.surface;

  return *this;
}

lpe::vulkan::Device::Device(Instance* instance,
                            PhysicalDevice* physicalDevice,
                            vk::PhysicalDevice vulkanPhysicalDevice,
                            vk::SurfaceKHR surface,
                            vk::PhysicalDeviceFeatures features)
  : surface(surface)
{
  this->instance.reset(instance);
  this->physicalDevice.reset(physicalDevice);

  auto queueInfos = physicalDevice->GetQueueFamilyIndices().Simplify();
  auto deviceExtenstions = Settings::Default().GetDeviceExtensions();
  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

  for (const auto& queueInfo : queueInfos)
  {
    queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(),
                                  queueInfo.familyIndex,
                                  static_cast<uint32_t>(queueInfo.prios.size()),
                                  !queueInfo.prios.empty() ?
                                    queueInfo.prios.data() :
                                    nullptr);
  }

  vk::DeviceCreateInfo createInfo = {};
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtenstions.size());
  createInfo.ppEnabledExtensionNames = deviceExtenstions.data();
  //createInfo.pEnabledFeatures = &features;

  auto validationLayers = Settings::Default().GetValidationLayers();
  if(Settings::Default().EnableValidationLayers)
  {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  device = vulkanPhysicalDevice.createDevice(createInfo,
                                             nullptr);
}

lpe::vulkan::Device::~Device()
{
  if(device)
  {
    device.destroy();
    device = nullptr;
  }
}

vk::Queue lpe::vulkan::Device::GetQueue(uint32_t familyIndex)
{
  auto iter = queues.find(familyIndex);
  if(iter != queues.end())
  {
    return iter->second;
  }

  vk::Queue queue = device.getQueue(familyIndex, 0);
  queues.insert(std::make_pair(familyIndex, queue));
  return queue;
}

lpe::vulkan::SwapChain lpe::vulkan::Device::CreateSwapChain(vk::PresentModeKHR presentMode,
                                                            vk::Format format,
                                                            vk::ColorSpaceKHR colorSpace)
{
  return { instance.get(), physicalDevice.get(), this, surface, presentMode, format, colorSpace };
}

lpe::vulkan::Device::operator vk::Device() const
{
  return device;
}

lpe::vulkan::Device::operator VkDevice() const
{
  return device;
}
