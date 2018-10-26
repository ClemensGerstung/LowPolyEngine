#include "ServiceLocator.h"
#include "RenderManager.h"

#include <set>
#include <map>

lpe::render::VulkanManager::VulkanManager()
  : device(nullptr),
    defaultSize(0)
{
}

void lpe::render::VulkanManager::Initialize()
{
  auto logger = ServiceLocator::LogManager.Get().lock();
  assert(logger);

  if (VK_API_VERSION_1_1 > vk::enumerateInstanceVersion())
  {
    logger->Log("Vulkan API 1.1 not supported! Check your drivers.");

    return;
  }

  vk::ApplicationInfo appInfo = {
    applicationName,
    applicationVersion,
    "src",
    VK_MAKE_VERSION(0, 1, 0),
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

  if (vk::createInstance(&instanceCreateInfo, nullptr, &instance) != vk::Result::eSuccess)
  {
    logger->Log("Could not create Instance. Is Vulkan even supported on your Device?");

    return;
  }

  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
  {
    logger->Log("Could not create Surface.");

    return;
  }

  this->surface = surface;

  PickPhysicalDevice();

  assert(this->physicalDevice);

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};
  std::map<uint32_t, uint32_t> queueCounts = {};
  float prio = 1.0;

  for (auto index : {queueIndices.computeFamilyIndex, queueIndices.presentFamilyIndex, queueIndices.graphicsFamilyIndex})
  {
    queueCounts[index.value()]++;
  }

  queueCreateInfos.reserve(queueCounts.size());
  for (auto &&pair : queueCounts)
  {
    queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlagBits(), pair.first, pair.second, &prio);
  }

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

  if (physicalDevice.createDevice(&createInfo, nullptr, &this->device) != vk::Result::eSuccess)
  {
    logger->Log("Could not create Device. Is Vulkan fully supported on your Device? (also check validation layers!)");

    return;
  }

  if (defaultSize == 0)
  {
    defaultSize = 128 * 1024 * 1024;  // 128 MiB
  }

  dynamicMemory.Create(device,
                       defaultSize);

  vk::BufferCreateInfo bufferCreateInfo = {
    {},
    defaultSize,
    vk::BufferUsageFlagBits::eTransferSrc
  };

  if (device.createBuffer(&bufferCreateInfo, nullptr, &stackAllocatorBuffer) != vk::Result::eSuccess)
  {
    logger->Log("Could not create StackAllocator's Buffer.");

    return;
  }

  auto memRequirements = device.getBufferMemoryRequirements(stackAllocatorBuffer);

  stackAllocator.Create(device, physicalDevice, defaultSize, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, memRequirements);
  stackAllocator.Push(physicalDevice, stackAllocatorBuffer);
}

void lpe::render::VulkanManager::Close()
{
  device.waitIdle();

  device.destroy();

  instance.destroySurfaceKHR(surface);
  instance.destroy();

  device = nullptr;
  physicalDevice = nullptr;
  surface = nullptr;
  instance = nullptr;
}

void lpe::render::VulkanManager::PickPhysicalDevice()
{
  assert(instance);
  auto physicalDevices = instance.enumeratePhysicalDevices();

  for (auto &&physicalDevice : physicalDevices)
  {
    auto extensions = physicalDevice.enumerateDeviceExtensionProperties();
    auto queueProperties = physicalDevice.getQueueFamilyProperties();
    swapchainDetails.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    swapchainDetails.formats = physicalDevice.getSurfaceFormatsKHR(surface);
    swapchainDetails.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
    std::vector<const char *> requiredExtensions = {deviceExtensions};

    for (uint32_t index = 0; index < queueProperties.size(); ++index)
    {
      auto &property = queueProperties[index];

      if (property.queueCount > 0 && (property.queueFlags & vk::QueueFlagBits::eGraphics))
      {
        queueIndices.graphicsFamilyIndex = index;
      }

      if (property.queueCount > 0 && (property.queueFlags & vk::QueueFlagBits::eCompute))
      {
        queueIndices.computeFamilyIndex = index;
      }

      auto supports = physicalDevice.getSurfaceSupportKHR(index, surface);

      if (property.queueCount > 0 && supports == VK_TRUE)
      {
        queueIndices.presentFamilyIndex = index;
      }

      if (queueIndices.graphicsFamilyIndex.has_value() &&
          queueIndices.presentFamilyIndex.has_value() &&
          queueIndices.computeFamilyIndex.has_value())
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

    if (swapchainDetails.capabilities.has_value() &&
        !swapchainDetails.formats.empty() &&
        !swapchainDetails.presentModes.empty() &&
        queueIndices.graphicsFamilyIndex.has_value() &&
        queueIndices.presentFamilyIndex.has_value() &&
        requiredExtensions.empty())
    {
      this->physicalDevice = physicalDevice;
      break;
    }
  }
}

void lpe::render::VulkanManager::Draw()
{
}

lpe::render::VulkanManager &lpe::render::VulkanManager::SetDefaultMemoryChunkSize(vk::DeviceSize defaultSize)
{
  return *this;
}

lpe::render::VulkanManager &lpe::render::VulkanManager::AddInstanceLayer(const char *layerName)
{
  this->layers.push_back(layerName);
  return *this;
}

lpe::render::VulkanManager &lpe::render::VulkanManager::AddInstanceExtension(const char *extensionName)
{
  this->instanceExtensions.push_back(extensionName);
  return *this;
}

lpe::render::VulkanManager &lpe::render::VulkanManager::AddDeviceExtension(const char *extensionName)
{
  return *this;
}

lpe::render::VulkanManager &lpe::render::VulkanManager::SetApplicationName(const char *applicationName)
{
  this->applicationName = applicationName;
  return *this;
}

lpe::render::VulkanManager &lpe::render::VulkanManager::SetApplicationVersion(uint16_t major, uint16_t minor, uint16_t patch)
{
  this->applicationVersion = VK_MAKE_VERSION(major, minor, patch);
  return *this;
}

bool lpe::render::VulkanManager::CheckInstanceExtensions()
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

bool lpe::render::VulkanManager::CheckInstanceLayers()
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

lpe::render::VulkanManager &lpe::render::VulkanManager::LinkGlfwWindow(GLFWwindow *window)
{
  this->window = window;
  return *this;
}

lpe::render::VulkanManager &lpe::render::VulkanManager::SetRequiredDeviceFeatures(const vk::PhysicalDeviceFeatures &features)
{
  this->requiredFeatures = features;
  return *this;
}

lpe::render::VulkanManager::operator vk::Device() const
{
  return this->device;
}


