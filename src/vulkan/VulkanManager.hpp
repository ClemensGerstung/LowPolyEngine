#ifndef LOWPOLYENGINE_VULKANMANAGER_HPP
#define LOWPOLYENGINE_VULKANMANAGER_HPP

#include "../RenderManager.h"
#include "../LogManager.h"


#include <vulkan/vulkan.hpp>

#define LPE_API_VERSION VK_MAKE_VERSION(0, 1, 0)

namespace lpe
{
namespace rendering
{
namespace vulkan
{

struct VulkanBase
{
  vk::Instance instance;
  vk::PhysicalDevice physicalDevice;
  VkSurfaceKHR surface; // using default Vulkan Handle because it's easier in this case...

  VulkanBase &operator=(std::nullptr_t);
};

struct VulkanQueue
{
  vk::Queue queue;
  uint32_t queueFamilyIndex;

  VulkanQueue &operator=(std::nullptr_t);
  VulkanQueue &operator=(vk::Queue queue);

  explicit operator vk::Queue() const;

  bool operator!() const;
};

struct VulkanDevice
{
  vk::Device device;
  VulkanQueue graphicsQueue;
  VulkanQueue presentQueue;
  VulkanQueue computeQueue;

  VulkanDevice &operator=(std::nullptr_t);
};

struct VulkanImage
{
  vk::Image image;
  vk::ImageView imageView;
};

struct VulkanSwapchain
{
  vk::SwapchainKHR swapchain;
  vk::SurfaceFormatKHR format;
  vk::Extent2D extent;
  vk::PresentModeKHR presentMode;
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<VulkanImage> images;

  VulkanSwapchain &operator=(std::nullptr_t);
};

class VulkanManager : public lpe::rendering::IRenderManager
{
private:
  VulkanBase base;
  VulkanDevice device;
  VulkanSwapchain swapchain;

  GLFWwindow *window;
  const char *applicationName;
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
};

} // vulkan
} // rendering
} // lpe

#endif //LOWPOLYENGINE_VULKANMANAGER_HPP
