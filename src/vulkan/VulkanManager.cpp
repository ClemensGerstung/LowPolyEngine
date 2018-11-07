#include <set>
#include <sstream>
#include <string>
#include "../utils.h"
#include "../ServiceLocator.h"
#include "VulkanBase.hpp"
#include "VulkanManager.hpp"

void lpe::rendering::vulkan::VulkanManager::Initialize()
{
  device.computeQueue = nullptr;
  device.graphicsQueue = nullptr;
  device.presentQueue = nullptr;

  this->logger = lpe::ServiceLocator::LogManager.Get().lock();
  assert(logger);

  assert(CreateInstance());

  assert(base.instance);

  if (glfwCreateWindowSurface(base.instance, window, nullptr, &base.surface) != VK_SUCCESS)
  {
    logger->Log("Could not create Surface.");

    return;
  }

  assert(PickPhysicalDevice());

  assert(base.physicalDevice);

  assert(CreateDeviceAndGetQueues());

  assert(device.device);
  assert(device.graphicsQueue);
  assert(device.presentQueue);

  assert(CreateSwapchain(vk::PresentModeKHR::eFifo,
                         vk::Format::eR8G8B8A8Unorm,
                         vk::ColorSpaceKHR::eSrgbNonlinear));

  assert(swapchain.swapchain);

  auto swapchainImages = device.device.getSwapchainImagesKHR(swapchain.swapchain);

  assert(swapchainImages.size() == swapchain.images.size());

  for(unsigned i = 0; i < swapchainImages.size(); ++i)
  {
    auto& image = swapchain.images[i];
    image.SetFormat(swapchain.format
                             .format)
         .SetViewType(vk::ImageViewType::e2D)
         .Create(lpe::utils::SimplePointer<VulkanManager>(this),
                 swapchainImages[i]);
  }
}

bool lpe::rendering::vulkan::VulkanManager::CreateInstance()
{
  if (VK_API_VERSION_1_1 > vk::enumerateInstanceVersion())
  {
    logger->Log("Vulkan API 1.1 not supported! Check your drivers.");

    return false;
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

    return false;
  }

  if (!CheckInstanceLayers())
  {
    logger->Log("Some required Layers are not supported.");

    return false;
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

    return false;
  }

  // TODO: Check if validation is enabled!
  VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
  debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
  debugReportCreateInfo.pfnCallback = DebugCallback;

//  common::CreateCallbackEXT<VkDebugReportCallbackCreateInfoEXT,
//    VkDebugReportCallbackEXT,
//    PFN_vkCreateDebugReportCallbackEXT>(base.instance,
//                                        &debugReportCreateInfo,
//                                        nullptr,
//                                        &debugReportCallback,
//                                        "vkCreateDebugReportCallbackEXT");


  localAllocator.Create(lpe::utils::SimplePointer<VulkanManager>(this), 128 * 1024 * 1024); // StackAllocator with 128 MiB

  return true;
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

  std::for_each(std::begin(swapchain.images),
                std::end(swapchain.images),
                [device = device.device](VulkanImage& image)
                {
                  image.Destroy();
                });

  device.device.destroySwapchainKHR(swapchain.swapchain);
  device.device.destroy();
  vkDestroySurfaceKHR(base.instance, base.surface, nullptr);
  base.instance.destroy();

  swapchain = nullptr;
  device = nullptr;
  base = nullptr;
}

bool lpe::rendering::vulkan::VulkanManager::PickPhysicalDevice()
{
  auto physicalDevices = base.instance.enumeratePhysicalDevices();

  for (auto &&physicalDevice : physicalDevices)
  {
    auto properties = physicalDevice.getProperties();
    std::stringstream string;
    string << "GPU: " << properties.deviceID << std::endl
           << "VendorID: " << properties.vendorID << std::endl
           << "DeviceID: " << properties.deviceID << std::endl
           << "DeviceType: " << vk::to_string(properties.deviceType) << std::endl
           << "API-Version: " << VK_VERSION_MAJOR(properties.apiVersion) << "."
                              << VK_VERSION_MINOR(properties.apiVersion) << "."
                              << VK_VERSION_PATCH(properties.apiVersion) << std::endl
           << "DeviceName: " << properties.deviceName  << std::endl
           << "Driver-Version: " << VK_VERSION_MAJOR(properties.driverVersion) << "."
                               << VK_VERSION_MINOR(properties.driverVersion) << "."
                               << VK_VERSION_PATCH(properties.driverVersion) << std::endl;

    logger->Log(string.str());


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

  if(!base.physicalDevice)
  {
    logger->Log("No matching physical device found");

    return false;
  }

  return true;
}

bool lpe::rendering::vulkan::VulkanManager::CreateDeviceAndGetQueues()
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

    return false;
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

  return true;
}

bool lpe::rendering::vulkan::VulkanManager::CreateSwapchain(vk::PresentModeKHR preferredMode,
                                                            vk::Format preferredFormat,
                                                            vk::ColorSpaceKHR preferredColorSpace)
{
  vk::SwapchainKHR old = swapchain.swapchain;
  if(old)
  {
    std::for_each(std::begin(swapchain.images),
                  std::end(swapchain.images),
                  [device = device.device](VulkanImage& image)
                  {
                    image.Destroy();
                  });
  }

  auto presentModes = base.physicalDevice.getSurfacePresentModesKHR(base.surface);
  auto formats = base.physicalDevice.getSurfaceFormatsKHR(base.surface);
  swapchain.capabilities = base.physicalDevice.getSurfaceCapabilitiesKHR(base.surface);

  auto iter = std::find_if(std::begin(presentModes),
                           std::end(presentModes),
                           [preferred = preferredMode](const vk::PresentModeKHR &mode)
                           {
                             return preferred == mode;
                           });

  swapchain.presentMode = (iter != std::end(presentModes)) ? preferredMode : vk::PresentModeKHR::eFifo;

  uint32_t imageCount = swapchain.capabilities.minImageCount + 1;
  if (swapchain.capabilities.maxImageCount > 0 && imageCount > swapchain.capabilities.maxImageCount)
  {
    imageCount = swapchain.capabilities.maxImageCount;
  }

  // if formats only contains one element which is VK_FORMAT_UNDEFINED we can choose whatever we want
  if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
  {
    swapchain.format = {
      preferredFormat,
      preferredColorSpace
    };
  }
  else
  {
    // simply use first format in list
    // if preferred format wasn't found at least we have a default format which we can use!
    // TODO: check if formats > 0?
    swapchain.format = formats[0];

    for (auto &&format : formats)
    {
      if (format.format == preferredFormat && format.colorSpace == preferredColorSpace)
      {
        swapchain.format = format;
        break;
      }
    }
  }

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  if (swapchain.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
  {
    swapchain.extent = swapchain.capabilities.currentExtent;
  }
  else
  {
    swapchain.extent = vk::Extent2D {
      static_cast<uint32_t >(width),
      static_cast<uint32_t >(height)
    };

    swapchain.extent.width = std::max(swapchain.capabilities.minImageExtent.width,
                                      std::max(swapchain.capabilities.maxImageExtent.width,
                                               swapchain.extent.width));
    swapchain.extent.height = std::max(swapchain.capabilities.minImageExtent.height,
                                       std::max(swapchain.capabilities.maxImageExtent.height,
                                                swapchain.extent.height));
  }

  vk::SwapchainCreateInfoKHR createInfo = {
    {},
    base.surface,
    imageCount,
    swapchain.format.format,
    swapchain.format.colorSpace,
    swapchain.extent,
    1,
    vk::ImageUsageFlagBits::eColorAttachment,
    vk::SharingMode::eExclusive,
    0,
    nullptr,
    swapchain.capabilities.currentTransform,
    vk::CompositeAlphaFlagBitsKHR::eOpaque,
    swapchain.presentMode,
    VK_TRUE,
    old
  };

  std::array<uint32_t, 2> queueFamilies = {
    device.presentQueue.queueFamilyIndex,
    device.graphicsQueue.queueFamilyIndex
  };

  if(device.presentQueue.queueFamilyIndex != device.graphicsQueue.queueFamilyIndex)
  {
    createInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
              .setQueueFamilyIndexCount(static_cast<uint32_t >(queueFamilies.size()))
              .setPQueueFamilyIndices(queueFamilies.data());
  }

  auto result = device.device.createSwapchainKHR(&createInfo, nullptr, &swapchain.swapchain);
  if(result != vk::Result::eSuccess)
  {
    logger->Log("Could not create swapchain.");

    return false;
  }

  swapchain.images.resize(imageCount);

  if(old)
  {
    device.device.destroySwapchainKHR(old, nullptr);
  }

  return true;
}

void lpe::rendering::vulkan::VulkanManager::Draw()
{

}

vk::Device lpe::rendering::vulkan::VulkanManager::GetDevice() const
{
  return device.device;
}

lpe::rendering::vulkan::VulkanManager& lpe::rendering::vulkan::VulkanManager::LinkGlfwWindow(GLFWwindow *window)
{
  VulkanManager::window = window;
  return *this;
}

lpe::rendering::vulkan::VulkanManager& lpe::rendering::vulkan::VulkanManager::SetApplicationName(const char *applicationName)
{
  strcpy(this->applicationName,
         applicationName);
  return *this;
}

lpe::rendering::vulkan::VulkanManager& lpe::rendering::vulkan::VulkanManager::SetApplicationVersion(uint16_t major, uint16_t minor, uint16_t patch)
{
  this->applicationVersion = VK_MAKE_VERSION(major, minor, patch);
  return *this;
}

lpe::rendering::vulkan::VulkanManager& lpe::rendering::vulkan::VulkanManager::AddInstanceLayer(const char *layerName)
{
  this->layers.emplace_back(layerName);
  return *this;
}

lpe::rendering::vulkan::VulkanManager& lpe::rendering::vulkan::VulkanManager::AddInstanceExtension(const char *extensionName)
{
  this->instanceExtensions.emplace_back(extensionName);
  return *this;
}

lpe::rendering::vulkan::VulkanManager& lpe::rendering::vulkan::VulkanManager::AddDeviceExtension(const char *extensionName)
{
  this->deviceExtensions.emplace_back(extensionName);
  return *this;
}

VkBool32 lpe::rendering::vulkan::VulkanManager::DebugCallback(VkDebugReportFlagsEXT flags,
                                                              VkDebugReportObjectTypeEXT objType,
                                                              uint64_t obj,
                                                              size_t location,
                                                              int32_t code,
                                                              const char *layerPrefix,
                                                              const char *msg,
                                                              void *userData)
{
  auto ptr = lpe::ServiceLocator::LogManager.Get()
                                            .lock();
  if (ptr)
  {
    // TODO: Check even more data
    ptr->Log(msg);
  }

  return VK_FALSE;
}

vk::PhysicalDevice lpe::rendering::vulkan::VulkanManager::GetPhysicalDevice() const
{
  return base.physicalDevice;
}

lpe::rendering::vulkan::StackAllocator& lpe::rendering::vulkan::VulkanManager::GetDeviceLocalMemory()
{
  return this->localAllocator;
}

lpe::rendering::vulkan::VulkanManager::QueueIndices::operator bool() const
{
  // compute queue is not explicitly required by now
  return graphicsQueueIndex.has_value() &&
         presentQueueIndex.has_value();
}

