#ifndef LOWPOLYENGINE_VULKANMANAGER_HPP
#define LOWPOLYENGINE_VULKANMANAGER_HPP

#include "../RenderManager.h"
#include "../LogManager.h"
#include "VulkanBase.hpp"
#include "MemoryManagement.hpp"

#define LPE_API_VERSION VK_MAKE_VERSION(0, 1, 0)

namespace lpe
{
namespace rendering
{
namespace vulkan
{

class VulkanManager : public lpe::rendering::IRenderManager
{
private:
  VulkanBase base;
  VulkanDevice device;
  VulkanSwapchain swapchain;

  StackAllocator localAllocator;

  VkDebugReportCallbackEXT debugReportCallback;

  GLFWwindow *window;
  char *applicationName;
  uint32_t applicationVersion;

  std::vector<const char *> instanceExtensions;
  std::vector<const char *> deviceExtensions;
  std::vector<const char *> layers;

  std::shared_ptr<lpe::utils::log::ILogManager> logger;

  struct QueueIndices
  {
    std::optional<uint32_t> graphicsQueueIndex;
    std::optional<uint32_t> presentQueueIndex;
    std::optional<uint32_t> computeQueueIndex;

    explicit operator bool() const;
  } queueIndices;

  VKAPI_ATTR static VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,
                                                      VkDebugReportObjectTypeEXT objType,
                                                      uint64_t obj,
                                                      size_t location,
                                                      int32_t code,
                                                      const char* layerPrefix,
                                                      const char* msg,
                                                      void* userData);


  bool CheckInstanceExtensions();

  bool CheckInstanceLayers();

  bool CreateInstance();

  bool PickPhysicalDevice();
  bool CreateDeviceAndGetQueues();

  bool CreateSwapchain(vk::PresentModeKHR preferredMode,
                       vk::Format preferredFormat,
                       vk::ColorSpaceKHR preferredColorSpace);
public:
  void Initialize() override;

  void Close() override;

  void Draw() override;

  VulkanManager& SetApplicationName(const char* applicationName);
  VulkanManager& SetApplicationVersion(uint16_t major, uint16_t minor, uint16_t patch);

  VulkanManager& SetDefaultMemoryChunkSize(vk::DeviceSize defaultSize);
  VulkanManager& AddInstanceLayer(const char* layerName);
  VulkanManager& AddInstanceExtension(const char* extensionName);
  VulkanManager& AddDeviceExtension(const char* extensionName);
  VulkanManager& LinkGlfwWindow(GLFWwindow* window);
  VulkanManager& SetRequiredDeviceFeatures(const vk::PhysicalDeviceFeatures& features);

  vk::Instance GetInstance() const;
  VkSurfaceKHR GetSurfaceKHR() const;
  vk::Device GetDevice() const;
  vk::PhysicalDevice GetPhysicalDevice() const;
  vk::SwapchainKHR GetSwapchain() const;
  StackAllocator& GetDeviceLocalMemory();
};

} // vulkan
} // rendering
} // lpe

#endif //LOWPOLYENGINE_VULKANMANAGER_HPP
