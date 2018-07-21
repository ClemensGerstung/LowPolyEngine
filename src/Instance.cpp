#include "lpe.h"

VkBool32 lpe::vulkan::Instance::DebugCallback(VkDebugReportFlagsEXT flags,
                                              VkDebugReportObjectTypeEXT objType,
                                              uint64_t obj,
                                              size_t location,
                                              int32_t code,
                                              const char* layerPrefix,
                                              const char* msg,
                                              void* userData)
{
  std::cerr << "validation layer: " << msg << std::endl;

  return VK_FALSE;
}

bool lpe::vulkan::Instance::CheckValidationLayerSupport() const
{
  auto availableLayers = vk::enumerateInstanceLayerProperties();
  auto requiredLayers = Settings::Default().GetValidationLayers();

  for (const auto& layerName : requiredLayers)
  {
    bool found = false;

    for (const auto& layerProp : availableLayers)
    {
      if (strcmp(layerName,
                 layerProp.layerName) == 0)
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      return false;
    }
  }

  return true;
}

lpe::vulkan::PhysicalDevice lpe::vulkan::Instance::GetSuitablePhysicalDevice(vk::SurfaceKHR surface,
                                                                             bool forceRequeryingPhysicalDevices)
{
  if (physicalDevices.empty() || forceRequeryingPhysicalDevices)
  {
    physicalDevices = instance.enumeratePhysicalDevices();
  }

  for (const auto& physicalDevice : physicalDevices)
  {
    PhysicalDevice device = { this, physicalDevice, surface };

    if (device.IsSuitable())
    {
      return device;
    }
  }

  throw std::runtime_error("failed to find suitable GPU!");
}

lpe::vulkan::Instance::Instance(const std::string&& name,
                                const uint32_t version)
{
  vk::ApplicationInfo applicationInfo =
  {
    name.c_str(),
    version,
    "LPE",
    LpeVersion,
    VK_MAKE_VERSION(1, 1, 0)
  };

  auto extensions = window::Window::GetRequiredVulkanExtensions();
  bool enableValidationLayers = Settings::Default().EnableValidationLayers;

  if (enableValidationLayers)
  {
    extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  }

  vk::InstanceCreateInfo createInfo =
  {
    {},
    &applicationInfo,
    0,
    nullptr,
    static_cast<uint32_t>(extensions.size()),
    extensions.data()
  };

  auto validationLayers = Settings::Default().GetValidationLayers();
  if (enableValidationLayers && CheckValidationLayerSupport())
  {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  auto result = vk::createInstance(&createInfo,
                                   nullptr,
                                   &instance);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create Vulkan instance");

  if (enableValidationLayers)
  {
    VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
    debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debugReportCreateInfo.pfnCallback = DebugCallback;

    if (helper::CreateCallbackEXT<VkDebugReportCallbackCreateInfoEXT,
                                  VkDebugReportCallbackEXT,
                                  PFN_vkCreateDebugReportCallbackEXT>(instance,
                                                                      &debugReportCreateInfo,
                                                                      nullptr,
                                                                      &debugReportCallback,
                                                                      "vkCreateDebugReportCallbackEXT"))
    {
      throw std::runtime_error("Failed to set up debug callback");
    }
  }
}

lpe::vulkan::Instance::~Instance()
{
  if (instance)
  {
    if (debugReportCallback)
    {
      helper::DestroyCallbackExt<VkDebugReportCallbackEXT,
                                 PFN_vkDestroyDebugReportCallbackEXT>(instance,
                                                                      debugReportCallback,
                                                                      nullptr,
                                                                      "vkDestroyDebugReportCallbackEXT");
      debugReportCallback = nullptr;
    }

    instance.destroy();
    instance = nullptr;
  }
}

std::vector<lpe::vulkan::PhysicalDevice> lpe::vulkan::Instance::GetPhysicalDevices(vk::SurfaceKHR surface,
                                                                                   bool forceRequeryingPhysicalDevices)
{
  if (physicalDevices.empty() || forceRequeryingPhysicalDevices)
  {
    physicalDevices = instance.enumeratePhysicalDevices();
  }

  std::vector<lpe::vulkan::PhysicalDevice> devices;

  for (const auto& physicalDevice : physicalDevices)
  {
    devices.emplace_back(this,
                         physicalDevice,
                         surface);
  }

  return devices;
}

lpe::vulkan::Instance::operator VkInstance() const
{
  return instance;
}

lpe::vulkan::Instance::operator vk::Instance() const
{
  return instance;
}
