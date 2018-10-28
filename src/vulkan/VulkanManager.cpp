#include <set>
#include "../ServiceLocator.h"
#include "VulkanManager.hpp"

void lpe::rendering::vulkan::VulkanManager::Initialize()
{
  device.computeQueue = nullptr;
  device.graphicsQueue = nullptr;
  device.presentQueue = nullptr;

  this->logger = lpe::ServiceLocator::LogManager.Get().lock();
  assert(logger);

  CreateInstance();

  assert(base.instance);

  if (glfwCreateWindowSurface(base.instance, window, nullptr, &base.surface) != VK_SUCCESS)
  {
    logger->Log("Could not create Surface.");

    return;
  }

  PickPhysicalDevice();

  assert(base.physicalDevice);

  CreateDeviceAndGetQueues();

  assert(device.device);
  assert(device.graphicsQueue);
  assert(device.presentQueue);


}

void lpe::rendering::vulkan::VulkanManager::CreateInstance()
{
  if (VK_API_VERSION_1_1 > vk::enumerateInstanceVersion())
  {
    logger->Log("Vulkan API 1.1 not supported! Check your drivers.");

    return;
  }

  vk::ApplicationInfo appInfo = {
    applicationName,
    applicationVersion,
    "lpe",
    LPE_API_VERSION,
    VK_API_VERSION_1_1
  };

  if (!CheckInstanceExtensions())
  {
    logger->Log("Some required Extensions are not supported.");

    return;
  }

  if (!CheckInstanceLayers())
  {
    logger->Log("Some required Layers are not supported.");

    return;
  }

  vk::InstanceCreateInfo instanceCreateInfo = {
    {},
    &appInfo,
    static_cast<uint32_t>(layers.size()),
    layers.data(),
    static_cast<uint32_t>(instanceExtensions.size()),
    instanceExtensions.data()
  };

  if (vk::createInstance(&instanceCreateInfo, nullptr, &base.instance) != vk::Result::eSuccess)
  {
    logger->Log("Could not create Instance. Is Vulkan even supported on your Device?");

    return;
  }
}

bool lpe::rendering::vulkan::VulkanManager::CheckInstanceExtensions()
{
  auto extensions = vk::enumerateInstanceExtensionProperties();
  bool result = true;

  for (auto &&required : instanceExtensions)
  {
    auto found = std::find_if(std::begin(extensions),
                              std::end(extensions),
                              [required = required](const vk::ExtensionProperties &extension)
                              {
                                return strcmp(required,
                                              extension.extensionName) == 0;
                              });

    result &= (std::end(extensions) != found);
  }

  return result;
}

bool lpe::rendering::vulkan::VulkanManager::CheckInstanceLayers()
{
  auto layerProperties = vk::enumerateInstanceLayerProperties();
  bool result = true;

  for (auto &&required : layers)
  {
    auto found = std::find_if(std::begin(layerProperties),
                              std::end(layerProperties),
                              [required = required](const vk::LayerProperties &layer)
                              {
                                return strcmp(required,
                                              layer.layerName) == 0;
                              });

    result &= (std::end(layerProperties) != found);
  }

  return result;
}

void lpe::rendering::vulkan::VulkanManager::Close()
{
  device.device.waitIdle();


  device.device.destroy();
  vkDestroySurfaceKHR(base.instance, base.surface, nullptr);
  base.instance.destroy();

  device = nullptr;
  base = nullptr;
}

void lpe::rendering::vulkan::VulkanManager::PickPhysicalDevice()
{
  auto physicalDevices = base.instance.enumeratePhysicalDevices();

  for (auto &&physicalDevice : physicalDevices)
  {
    auto queueProperties = physicalDevice.getQueueFamilyProperties();
    auto extensions = physicalDevice.enumerateDeviceExtensionProperties();
    std::vector<const char *> requiredExtensions = {deviceExtensions};

    for (uint32_t index = 0; index < queueProperties.size(); ++index)
    {
      auto &property = queueProperties[index];

      if (property.queueCount > 0 && (property.queueFlags & vk::QueueFlagBits::eGraphics))
      {
        queueIndices.graphicsQueueIndex = index;
      }

      if (property.queueCount > 0 && (property.queueFlags & vk::QueueFlagBits::eCompute))
      {
        queueIndices.computeQueueIndex = index;
      }

      auto supports = physicalDevice.getSurfaceSupportKHR(index, base.surface);

      if (property.queueCount > 0 && supports == VK_TRUE)
      {
        queueIndices.presentQueueIndex = index;
      }

      if (queueIndices)
      {
        break;
      }

      index++;
    }

    for (auto &&extension : extensions)
    {
      auto iter = std::find_if(std::begin(requiredExtensions),
                               std::end(requiredExtensions),
                               [required = extension.extensionName](const char *extensionName)
                               {
                                 return strcmp(required, extensionName) == 0;
                               });

      if (iter != std::end(requiredExtensions))
      {
        requiredExtensions.erase(iter);
      }
    }

    // TODO: maybe check if discrete GPU?
//    auto properties = physicalDevice.getProperties();
//    if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
//    {
//      //???
//    }

    if (queueIndices &&
        requiredExtensions.empty())
    {
      base.physicalDevice = physicalDevice;
      break;
    }
  }
}

void lpe::rendering::vulkan::VulkanManager::CreateDeviceAndGetQueues()
{

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};
  std::set<uint32_t> queueFamilies =
    {
      queueIndices.computeQueueIndex.value(),
      queueIndices.presentQueueIndex.value(),
      queueIndices.graphicsQueueIndex.value()
    };
  float prio = 1.0;

  queueCreateInfos.reserve(queueFamilies.size());
  for (auto &&index : queueFamilies)
  {
    queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlagBits(), index, 1, &prio);
  }

  vk::PhysicalDeviceFeatures requiredFeatures = {};
  requiredFeatures.samplerAnisotropy = VK_TRUE;

  vk::DeviceCreateInfo createInfo = {
    {},
    static_cast<uint32_t >(queueCreateInfos.size()),
    queueCreateInfos.data(),
    0,
    nullptr,
    static_cast<uint32_t>(deviceExtensions.size()),
    deviceExtensions.data(),
    &requiredFeatures
  };

  if (base.physicalDevice.createDevice(&createInfo, nullptr, &device.device) != vk::Result::eSuccess)
  {
    logger->Log("Could not create Device. Is Vulkan fully supported on your Device? (also check validation layers!)");

    return;
  }

  if (queueIndices.graphicsQueueIndex)
  {
    uint32_t &index = queueIndices.graphicsQueueIndex.value();
    device.graphicsQueue = device.device.getQueue(index, 0);
    device.graphicsQueue.queueFamilyIndex = index;
  }

  // TODO: maybe use other queue index for present
  if (queueIndices.presentQueueIndex)
  {
    uint32_t &index = queueIndices.presentQueueIndex.value();
    device.presentQueue = device.device.getQueue(index, 0);
    device.presentQueue.queueFamilyIndex = index;
  }

  // TODO: maybe use other queue index for compute
  if (queueIndices.computeQueueIndex)
  {
    uint32_t &index = queueIndices.computeQueueIndex.value();
    device.computeQueue = device.device.getQueue(index, 0);
    device.computeQueue.queueFamilyIndex = index;
  }
}

lpe::rendering::vulkan::VulkanQueue &lpe::rendering::vulkan::VulkanQueue::operator=(std::nullptr_t)
{
  queue = nullptr;
  queueFamilyIndex = 0;

  return *this;
}

lpe::rendering::vulkan::VulkanManager::QueueIndices::operator bool() const
{
  // compute queue is not explicitly required by now
  return graphicsQueueIndex.has_value() &&
         presentQueueIndex.has_value();
}

lpe::rendering::vulkan::VulkanBase &lpe::rendering::vulkan::VulkanBase::operator=(std::nullptr_t)
{
  instance = nullptr;
  physicalDevice = nullptr;
  surface = nullptr;

  return *this;
}

lpe::rendering::vulkan::VulkanDevice &lpe::rendering::vulkan::VulkanDevice::operator=(std::nullptr_t)
{
  device = nullptr;
  graphicsQueue = nullptr;
  presentQueue = nullptr;
  computeQueue = nullptr;

  return *this;
}

lpe::rendering::vulkan::VulkanQueue::operator vk::Queue() const
{
  return queue;
}

bool lpe::rendering::vulkan::VulkanQueue::operator!() const
{
  return !queue;
}

lpe::rendering::vulkan::VulkanQueue &lpe::rendering::vulkan::VulkanQueue::operator=(vk::Queue queue)
{
  this->queue = queue;
  return *this;
}
