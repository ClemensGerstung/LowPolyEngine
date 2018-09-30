#include "RenderManager.h"

lpe::render::VulkanManager::VulkanManager()
  : device(nullptr),
    defaultSize(0)
{
}

void lpe::render::VulkanManager::Initialize()
{
  vk::ApplicationInfo appInfo = {
    "tbd.",
    0,
    "lpe",
    VK_MAKE_VERSION(0, 1, 0),
    VK_API_VERSION_1_1
  };

  auto versions = vk::enumerateInstanceVersion();
  auto extensions = vk::enumerateInstanceExtensionProperties();
  auto layers = vk::enumerateInstanceLayerProperties();

  std::vector<const char*> requiredExtensions = {};
  requiredExtensions.push_back("VK_KHR_device_group_creation");
  requiredExtensions.push_back("VK_KHR_surface");
  requiredExtensions.push_back("VK_KHR_win32_surface");
  //requiredExtensions.push_back("VK_EXT_debug_report");

  vk::InstanceCreateInfo instanceCreateInfo = {
    {},
    &appInfo,
    0,
    nullptr,
    static_cast<uint32_t>(requiredExtensions.size()),
    requiredExtensions.data()
  };

  assert(vk::createInstance(&instanceCreateInfo, nullptr, &instance) == vk::Result::eSuccess);

  uint32_t count;
  vkEnumeratePhysicalDeviceGroups(instance, &count, nullptr);

  std::vector<VkPhysicalDeviceGroupProperties> props = {};
  props.resize(count);
  vkEnumeratePhysicalDeviceGroups(instance, &count, props.data());

  auto physicalDevices = instance.enumeratePhysicalDevices();

  vk::DeviceCreateInfo info = {
  
  };

  VkDeviceGroupDeviceCreateInfoKHR deviceGroupInfo = { VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO_KHR };
  if (props.size() > 0 && 
      props[0].physicalDeviceCount > 1) {
    deviceGroupInfo.physicalDeviceCount = props[0].physicalDeviceCount;
    deviceGroupInfo.pPhysicalDevices = props[0].physicalDevices;

    info.pNext = &deviceGroupInfo;
  }

  if (defaultSize == 0)
  {
    defaultSize = 128 * 1024 * 1024;  // 128 MiB
  }

  dynamicMemory.Create(device, 
                       defaultSize);
}

void lpe::render::VulkanManager::Close()
{
}

void lpe::render::VulkanManager::Draw()
{
}

lpe::render::VulkanManager & lpe::render::VulkanManager::SetDefaultMemoryChunkSize(vk::DeviceSize defaultSize)
{
  return *this;
}

lpe::render::VulkanManager & lpe::render::VulkanManager::AddLayer(const char * layerName)
{
  return *this;
}

lpe::render::VulkanManager & lpe::render::VulkanManager::AddExtension(const char * extensionName)
{
  return *this;
}
