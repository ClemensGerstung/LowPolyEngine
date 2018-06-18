#include "PhysicalDevice.h"
#include "Device.h"

bool lpe::vulkan::QueueInfo::operator==(const QueueInfo& other) const
{
  return familyIndex == other.familyIndex;
}

bool lpe::vulkan::QueueInfo::operator!=(const QueueInfo& other) const
{
  return familyIndex != other.familyIndex;
}

bool lpe::vulkan::QueueInfo::operator<(const QueueInfo& other) const
{
  return familyIndex < other.familyIndex;
}

bool lpe::vulkan::QueueFamilyIndices::IsComplete() const
{
  return !graphicsFamily.prios.empty() && !presentFamily.prios.empty();
}

std::set<lpe::vulkan::QueueInfo> lpe::vulkan::QueueFamilyIndices::Simplify() const
{
  return { graphicsFamily, presentFamily, computeFamily };
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
  surfaceCapabilities = other.surfaceCapabilities;
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
  surfaceCapabilities = other.surfaceCapabilities;
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
  surfaceCapabilities = other.surfaceCapabilities;

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
  surfaceCapabilities = other.surfaceCapabilities;

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
  this->surfaceCapabilities = this->physicalDevice.getSurfaceCapabilitiesKHR(this->surface);
  auto memprops = this->physicalDevice.getMemoryProperties();
}

lpe::vulkan::PhysicalDevice::~PhysicalDevice()
{
  if (surface && instance)
  {
    vkDestroySurfaceKHR(*instance,
                        surface,
                        nullptr);
    surface = nullptr;
  }
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
        queueFamilyIndices.graphicsFamily
                          .familyIndex = index;
        queueFamilyIndices.graphicsFamily
                          .prios
                          .resize(queueFamily.queueCount,
                                  1.0f);
      }

      if (queueFamily.queueCount > 0 &&
        queueFamily.queueFlags & requiredFlags & vk::QueueFlagBits::eCompute)
      {
        queueFamilyIndices.computeFamily
                          .familyIndex = index;
        queueFamilyIndices.computeFamily
                          .prios
                          .resize(queueFamily.queueCount,
                                  1.0f);
      }

      vk::Bool32 presentSupport = physicalDevice.getSurfaceSupportKHR(index,
                                                                      surface);

      if (queueFamily.queueCount > 0 && presentSupport)
      {
        queueFamilyIndices.presentFamily.familyIndex = index;
        queueFamilyIndices.presentFamily
                          .prios
                          .resize(queueFamily.queueCount,
                                  1.0f);
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

lpe::vulkan::Device lpe::vulkan::PhysicalDevice::CreateDevice(vk::PhysicalDeviceFeatures features)
{
  return { instance.get(), this, physicalDevice, surface, features };
}

uint32_t lpe::vulkan::PhysicalDevice::GetSwapChainImageCount() const
{
  uint32_t numberOfImages = surfaceCapabilities.minImageCount + 1;

  if (surfaceCapabilities.maxImageCount > 0 &&
    numberOfImages > surfaceCapabilities.maxImageCount)
  {
    numberOfImages = surfaceCapabilities.maxImageCount;
  }

  return numberOfImages;
}

vk::Extent2D lpe::vulkan::PhysicalDevice::GetSwapChainImageExtent() const
{
  vk::Extent2D extent;
  if (surfaceCapabilities.currentExtent.width == 0xffffffff)
  {
    extent = vk::Extent2D(1280,
                          720);

    if (extent.width < surfaceCapabilities.minImageExtent.width)
    {
      extent.width = surfaceCapabilities.minImageExtent.width;
    }
    else if (extent.width > surfaceCapabilities.maxImageExtent.width)
    {
      extent.width = surfaceCapabilities.maxImageExtent.width;
    }

    if (extent.height < surfaceCapabilities.minImageExtent.height)
    {
      extent.height = surfaceCapabilities.minImageExtent.height;
    }
    else if (extent.height > surfaceCapabilities.maxImageExtent.height)
    {
      extent.height = surfaceCapabilities.maxImageExtent.height;
    }
  }
  else
  {
    extent = surfaceCapabilities.currentExtent;
  }

  return extent;
}

bool lpe::vulkan::PhysicalDevice::SupportsSurfaceCapability(vk::ImageUsageFlags flags) const
{
  return (surfaceCapabilities.supportedUsageFlags & flags) == flags;
}

bool lpe::vulkan::PhysicalDevice::SupportsSurfaceTransform(vk::SurfaceTransformFlagsKHR flags) const
{
  return (surfaceCapabilities.supportedTransforms & flags) == flags;
}

void lpe::vulkan::PhysicalDevice::SupportsSurfaceFormat(vk::Format* format,
                                                        vk::ColorSpaceKHR* colorSpace) const
{
  auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

  if(surfaceFormats.size() == 1 && 
    surfaceFormats[0].format == vk::Format::eUndefined)
  {
    return;
  }

  for(const auto& surfaceFormat : surfaceFormats)
  {
    if(*format == surfaceFormat.format && *colorSpace == surfaceFormat.colorSpace)
    {
      return;
    }
  }

  for (const auto& surfaceFormat : surfaceFormats)
  {
    if (*format == surfaceFormat.format)
    {
      *colorSpace = surfaceFormat.colorSpace;
      return;
    }
  }

  if(!surfaceFormats.empty())
  {
    *format = surfaceFormats[0].format;
    *colorSpace = surfaceFormats[0].colorSpace;
    return;
  }

  throw std::runtime_error("could not find any format and colorspace");
}

lpe::vulkan::PhysicalDevice::operator vk::PhysicalDevice() const
{
  return physicalDevice;
}

lpe::vulkan::PhysicalDevice::operator VkPhysicalDevice() const
{
  return physicalDevice;
}
