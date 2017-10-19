#include "../include/Instance.h"
#include <set>

lpe::QueueFamilyIndices lpe::Instance::FindQueueFamilies(vk::PhysicalDevice device, const vk::SurfaceKHR& surface)
{
  lpe::QueueFamilyIndices indices;
  auto queueFamilies = device.getQueueFamilyProperties();

  int index = 0;

  for (const auto& queueFamily : queueFamilies)
  {
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
    {
      indices.graphicsFamily = index;
    }

    vk::Bool32 presentSupport = device.getSurfaceSupportKHR(index, surface);

    if (queueFamily.queueCount > 0 && presentSupport)
    {
      indices.presentFamily = index;
    }

    if (indices.IsComplete())
    {
      break;
    }

    index++;
  }

  return indices;
}

bool lpe::Instance::CheckDeviceExtensionSupport(vk::PhysicalDevice device)
{
  auto extensions = device.enumerateDeviceExtensionProperties();

  std::set<std::string> requiredExtensions(helper::DeviceExtensions.begin(), helper::DeviceExtensions.end());

  for (const auto& extension : extensions)
  {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

SwapChainSupportDetails lpe::Instance::QuerySwapChainDetails(vk::PhysicalDevice device, const vk::SurfaceKHR& surface)
{
  SwapChainSupportDetails details;

  details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
  details.formats = device.getSurfaceFormatsKHR(surface);
  details.presentModes = device.getSurfacePresentModesKHR(surface);

  return details;
}

bool lpe::Instance::IsDeviceSuitable(vk::PhysicalDevice device, const vk::SurfaceKHR& surface) const
{
  auto indices = FindQueueFamilies(device, surface);

  bool supportsExtensions = CheckDeviceExtensionSupport(device);

  bool isSwapChainAdequate = false;
  if (supportsExtensions)
  {
    auto swapChainDetails = QuerySwapChainDetails(device, surface);
    isSwapChainAdequate = !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
  }

  return indices.IsComplete() && supportsExtensions && isSwapChainAdequate;
}

vk::PhysicalDevice lpe::Instance::PickPhysicalDevice(const uint32_t physicalDeviceIndex, const vk::SurfaceKHR& surface)
{
  vk::PhysicalDevice physicalDevice;

  auto physicalDevices = instance.enumeratePhysicalDevices();

  if (physicalDeviceIndex == -1)
  {
    for (const auto& device : physicalDevices)
    {
      if (IsDeviceSuitable(device, surface))
      {
        physicalDevice = device;
        break;
      }
    }
  }
  else
  {
    if (IsDeviceSuitable(physicalDevices[physicalDeviceIndex], surface))
    {
      physicalDevice = physicalDevices[physicalDeviceIndex];
    }
  }

  if (!physicalDevice)
  {
    throw std::runtime_error("failed to find suitable GPU!");
  }

  this->physicalDevices.push_back(physicalDevice);
  return physicalDevice;
}

lpe::Instance::~Instance()
{
  if (callback)
  {
    helper::DestroyDebugReportCallbackEXT(instance, callback, nullptr);
  }

  if (instance)
  {
    instance.destroy(nullptr);
  }
}

void lpe::Instance::Create(const std::string& appName)
{
  if (settings.EnableValidationLayer && !helper::CheckValidationLayerSupport())
  {
    throw std::runtime_error("validation layers requested, but not available");
  }

  std::vector<const char*> extensions;

  unsigned int glfwExtensionCount = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  for (unsigned int i = 0; i < glfwExtensionCount; i++)
  {
    extensions.push_back(glfwExtensions[i]);
  }

  if (settings.EnableValidationLayer)
  {
    extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  }

  vk::ApplicationInfo appInfo = {
    appName.c_str(), APPLICATION_VERSION, ENGINE_NAME, ENGINE_VERSION, VK_MAKE_VERSION(1, 0, 39)
  };

  vk::InstanceCreateInfo createInfo =
  {
    {},
    &appInfo,
    0,
    nullptr,
    (uint32_t)extensions.size(),
    extensions.data()
  };

  if(settings.EnableValidationLayer)
  {
    createInfo.enabledLayerCount = (uint32_t)helper::ValidationLayer.size();
    createInfo.ppEnabledLayerNames = helper::ValidationLayer.data();
  }

  auto result = vk::createInstance(&createInfo, nullptr, &instance);
  helper::ThrowIfNotSuccess(result, "Couldn't create Vulkan instance");

  if (settings.EnableValidationLayer)
  {
    VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
    debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debugReportCreateInfo.pfnCallback = helper::debugCallback;

    if (helper::CreateDebugReportCallbackEXT(instance, &debugReportCreateInfo, nullptr, &callback))
    {
      throw std::runtime_error("Failed to set up debug callback");
    }
  }
}

lpe::Device lpe::Instance::CreateDevice(GLFWwindow* window, const uint32_t physicalDeviceIndex)
{
  vk::SurfaceKHR surface;

  if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), window, nullptr,
                              reinterpret_cast<VkSurfaceKHR*>(&surface)) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create window surface!");
  }

  vk::PhysicalDevice physicalDevice = PickPhysicalDevice(physicalDeviceIndex, surface);

  return {&instance, physicalDevice, surface};
}
