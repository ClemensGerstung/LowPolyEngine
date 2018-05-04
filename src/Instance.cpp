#include "Instance.h"

lpe::vulkan::Instance::Instance(const std::string&& name,
                                const uint32_t version)
{
  vk::ApplicationInfo applicationInfo =
  {
    name.c_str(),
    version,
    "LPE",
    VK_MAKE_VERSION(0, 1, 0),
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

  if (enableValidationLayers)
  {
    auto validationLayers = Settings::Default().GetValidationLayers();
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

lpe::vulkan::Instance::operator VkInstance() const
{
  return instance;
}

lpe::vulkan::Instance::operator vk::Instance() const
{
  return instance;
}
