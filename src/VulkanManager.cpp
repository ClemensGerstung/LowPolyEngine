#include "VulkanManager.h"
#include "Window.h"

#if _DEBUG
bool lpe::vulkan::VulkanManager::EnableValidationLayers = true;
#else
bool lpe::vulkan::VulkanManager::EnableValidationLayers = true;
#endif

VkBool32 lpe::vulkan::VulkanManager::DebugCallback(VkDebugReportFlagsEXT flags,
                                                   VkDebugReportObjectTypeEXT objType,
                                                   uint64_t obj,
                                                   size_t location,
                                                   int32_t code,
                                                   const char* layerPrefix,
                                                   const char* msg,
                                                   void* userData)
{
  lpe::ServiceLocator::Default.Logger->Write(msg);

  return VK_FALSE;
}


void lpe::vulkan::VulkanManager::Initialize()
{
  validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");

  auto requiredWindowExtensions = lpe::window::Window::GetRequiredVulkanExtensions();

  instanceExtensions.insert(std::end(instanceExtensions),
                            std::begin(requiredWindowExtensions),
                            std::end(requiredWindowExtensions));

  if (EnableValidationLayers)
  {
    instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  }

  deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void lpe::vulkan::VulkanManager::Destroy()
{
}

void lpe::vulkan::VulkanManager::SetValidationLayers(const std::vector<const char*>& layers)
{
  this->validationLayers = { layers };
}

void lpe::vulkan::VulkanManager::SetValidationLayers(const char ** layers, uint32_t layersCount)
{
  validationLayers.clear();
  validationLayers.resize(layersCount);
  validationLayers.insert(std::end(validationLayers),
                          &layers[0],
                          &layers[layersCount]);
}

void lpe::vulkan::VulkanManager::SetDeviceExtensions(const std::vector<const char*>& extensionNames)
{
  deviceExtensions = { extensionNames };
}

void lpe::vulkan::VulkanManager::SetDeviceExtensions(const char ** extensionNames, uint32_t extensionCount)
{
  deviceExtensions.clear();
  deviceExtensions.resize(extensionCount);
  deviceExtensions.insert(std::end(deviceExtensions),
                          &extensionNames[0],
                          &extensionNames[extensionCount]);
}

void lpe::vulkan::VulkanManager::SetInstanceExtensions(const std::vector<const char*>& extensionNames)
{
  instanceExtensions = { extensionNames };
}

void lpe::vulkan::VulkanManager::SetInstanceExtensions(const char ** extensionNames, uint32_t extensionCount)
{
  instanceExtensions.clear();
  instanceExtensions.resize(extensionCount);
  instanceExtensions.insert(std::end(instanceExtensions),
                            &extensionNames[0],
                            &extensionNames[extensionCount]);
}
