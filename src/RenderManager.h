#pragma once

#include "ServiceBase.h"
#include "VkMemoryManagement.h"
#include "utils.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace render
  {
    class IRenderManager : public ServiceBase
    {
    public:
      ~IRenderManager() override = default;

      virtual void Draw() = 0;
    };

    class VulkanManager : public IRenderManager
    {
    private:
      vk::Instance instance;
      vk::SurfaceKHR surface;
      vk::PhysicalDevice physicalDevice;
      vk::Device device;
      vk::DeviceSize defaultSize;
      VkMemoryManagement dynamicMemory;
      VkStackAllocator stackAllocator;

      std::vector<const char*> instanceExtensions;
      std::vector<const char*> deviceExtensions;
      std::vector<const char*> layers;

      GLFWwindow* window;
      const char* applicationName;
      uint32_t applicationVersion;

      struct {
        std::optional<uint32_t> graphicsFamilyIndex;
        std::optional<uint32_t> presentFamilyIndex;
        std::optional<uint32_t> computeFamilyIndex;
      } queueIndices;

      struct {
        std::optional<vk::SurfaceCapabilitiesKHR> capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
      } swapchainDetails;

      bool CheckInstanceExtensions();
      bool CheckInstanceLayers();
      void PickPhysicalDevice();

    public:
      VulkanManager();
      
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

      lpe::utils::SimplePointer<VkMemoryManagement> GetDeviceMemory() const;
      lpe::utils::SimplePointer<VkStackAllocator> GetHostMemory() const;
    };
  }
}
