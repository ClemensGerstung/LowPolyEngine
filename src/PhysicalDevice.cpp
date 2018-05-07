#include "PhysicalDevice.h"

bool lpe::vulkan::QueueFamilyIndices::IsComplete() const
{
  return graphicsFamily >= 0 && presentFamily >= 0;
}

std::array<int32_t, 2> lpe::vulkan::QueueFamilyIndices::ToArray() const
{
  return { graphicsFamily, presentFamily };
}

bool lpe::vulkan::SwapChainSupportDetails::IsSuitable() const
{
  return !formats.empty() && !presentModes.empty();
}

lpe::vulkan::PhysicalDevice::PhysicalDevice(const PhysicalDevice& other)
{
  instance.reset(other.instance.get());
  physicalDevice = other.physicalDevice;
  surface = other.surface;
  properties = other.properties;
  features = other.features;
  memoryProperties = other.memoryProperties;
  availableDeviceExtensions = { other.availableDeviceExtensions };
  swapChainSupportDetails = other.swapChainSupportDetails;
  queueFamilyIndices = other.queueFamilyIndices;
}

lpe::vulkan::PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
{
  instance = std::move(other.instance);
  physicalDevice = other.physicalDevice;
  surface = other.surface;
  properties = other.properties;
  features = other.features;
  memoryProperties = other.memoryProperties;
  availableDeviceExtensions = std::move(other.availableDeviceExtensions);
  swapChainSupportDetails = other.swapChainSupportDetails;
  queueFamilyIndices = other.queueFamilyIndices;
}

lpe::vulkan::PhysicalDevice& lpe::vulkan::PhysicalDevice::operator=(const PhysicalDevice& other)
{
  instance.reset(other.instance.get());
  physicalDevice = other.physicalDevice;
  surface = other.surface;
  properties = other.properties;
  features = other.features;
  memoryProperties = other.memoryProperties;
  availableDeviceExtensions = { other.availableDeviceExtensions };
  swapChainSupportDetails = other.swapChainSupportDetails;
  queueFamilyIndices = other.queueFamilyIndices;

  return *this;
}

lpe::vulkan::PhysicalDevice& lpe::vulkan::PhysicalDevice::operator=(PhysicalDevice&& other) noexcept
{
  instance = std::move(other.instance);
  physicalDevice = other.physicalDevice;
  surface = other.surface;
  properties = other.properties;
  features = other.features;
  memoryProperties = other.memoryProperties;
  availableDeviceExtensions = std::move(other.availableDeviceExtensions);
  swapChainSupportDetails = other.swapChainSupportDetails;
  queueFamilyIndices = other.queueFamilyIndices;

  return *this;
}

lpe::vulkan::PhysicalDevice::PhysicalDevice(lpe::vulkan::Instance* instance,
                                            vk::PhysicalDevice physicalDevice,
                                            vk::SurfaceKHR surface)
{
  this->instance.reset(instance);
  this->physicalDevice = physicalDevice;
  this->surface = surface;
  this->properties = this->physicalDevice.getProperties();
  this->features = this->physicalDevice.getFeatures();
  this->memoryProperties = this->physicalDevice.getMemoryProperties();
}

std::vector<vk::ExtensionProperties> lpe::vulkan::PhysicalDevice::GetDeviceExtensions(bool forceRequerying)
{
  if (availableDeviceExtensions.empty() || forceRequerying)
  {
    availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
  }

  return availableDeviceExtensions;
}

bool lpe::vulkan::PhysicalDevice::SupportsExtensions(bool forceRequerying)
{
  if (availableDeviceExtensions.empty() || forceRequerying)
  {
    availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
  }

  std::vector<const char*> requieredExtensions = Settings::Default().GetDeviceExtensions();
  std::set<std::string> set = { requieredExtensions.begin(), requieredExtensions.end() };

  for (const auto& extension : availableDeviceExtensions)
  {
    set.erase(extension.extensionName);
  }

  return set.empty();
}

lpe::vulkan::SwapChainSupportDetails lpe::vulkan::PhysicalDevice::QuerySwapChainSupportDetails(bool forceRequerying)
{
  if (!swapChainSupportDetails.IsSuitable() || forceRequerying)
  {
    swapChainSupportDetails.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    swapChainSupportDetails.formats = physicalDevice.getSurfaceFormatsKHR(surface);
    swapChainSupportDetails.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
  }

  return swapChainSupportDetails;
}

lpe::vulkan::QueueFamilyIndices lpe::vulkan::PhysicalDevice::GetQueueFamilyIndices(vk::QueueFlags requiredFlags,
                                                                                   bool forceRequerying)
{
  if (!queueFamilyIndices.IsComplete() || forceRequerying)
  {
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    int32_t index = 0;

    for (const auto& queueFamily : queueFamilies)
    {
      if (queueFamily.queueCount > 0 && 
          queueFamily.queueFlags & requiredFlags & vk::QueueFlagBits::eGraphics)
      {
        queueFamilyIndices.graphicsFamily = index;
      }

      if (queueFamily.queueCount > 0 &&
          queueFamily.queueFlags & requiredFlags & vk::QueueFlagBits::eCompute)
      {
        queueFamilyIndices.graphicsFamily = index;
      }

      vk::Bool32 presentSupport = physicalDevice.getSurfaceSupportKHR(index,
                                                                      surface);

      if (queueFamily.queueCount > 0 && presentSupport)
      {
        queueFamilyIndices.presentFamily = index;
      }

      if (queueFamilyIndices.IsComplete())
      {
        break;
      }

      index++;
    }
  }

  return queueFamilyIndices;
}

bool lpe::vulkan::PhysicalDevice::IsSuitable(vk::QueueFlags requiredFlags)
{
  GetQueueFamilyIndices(requiredFlags);
  bool supported = SupportsExtensions();
  if (supported)
  {
    QuerySwapChainSupportDetails();
  }

  return queueFamilyIndices.IsComplete() && supported && swapChainSupportDetails.IsSuitable();
}

uint32_t lpe::vulkan::PhysicalDevice::FindMemoryTypeIndex(uint32_t typeFilter,
                                                          vk::MemoryPropertyFlags propertyFlags) const
{
  for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
  {
    if ((typeFilter & (1 << i)) &&
      (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags))
    {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}
