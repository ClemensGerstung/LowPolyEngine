#pragma once

#include <set>
#include <vulkan/vulkan.hpp>
#include "Instance.h"

namespace lpe
{
  namespace vulkan
  {
    class Device;

    struct QueueInfo
    {
      uint32_t familyIndex;
      std::vector<float> prios;

      bool operator==(const QueueInfo& other) const;
      bool operator!=(const QueueInfo& other) const;
      bool operator<(const QueueInfo& other) const;
    };

    struct QueueFamilyIndices
    {
      QueueInfo graphicsFamily = {};
      QueueInfo presentFamily = {};
      QueueInfo computeFamily = {};
      QueueInfo transferFamily = {};

      bool IsComplete() const;
      std::set<QueueInfo> Simplify() const;
    };

    struct SwapChainSupportDetails
    {
      vk::SurfaceCapabilitiesKHR capabilities;
      std::vector<vk::SurfaceFormatKHR> formats;
      std::vector<vk::PresentModeKHR> presentModes;

      bool IsSuitable() const;
    };

    class PhysicalDevice
    {
    private:
      vk::PhysicalDeviceProperties properties;
      vk::PhysicalDeviceFeatures features;
      vk::PhysicalDeviceMemoryProperties memoryProperties;

      vk::PhysicalDevice physicalDevice;
      helper::Pointer<Instance> instance;
      vk::SurfaceKHR surface;

      std::vector<vk::ExtensionProperties> availableDeviceExtensions;
      SwapChainSupportDetails swapChainSupportDetails;
      QueueFamilyIndices queueFamilyIndices;
    public:
      PhysicalDevice() = default;
      PhysicalDevice(const PhysicalDevice& other);
      PhysicalDevice(PhysicalDevice&& other) noexcept;
      PhysicalDevice& operator=(const PhysicalDevice& other);
      PhysicalDevice& operator=(PhysicalDevice&& other) noexcept;
      PhysicalDevice(lpe::vulkan::Instance* instance,
                     vk::PhysicalDevice physicalDevice,
                     vk::SurfaceKHR surface);

      ~PhysicalDevice();

      std::vector<vk::ExtensionProperties> GetDeviceExtensions(bool forceRequerying = false);
      bool SupportsExtensions(bool forceRequerying = false);
      SwapChainSupportDetails QuerySwapChainSupportDetails(bool forceRequerying = false);
      QueueFamilyIndices GetQueueFamilyIndices(vk::QueueFlags requiredFlags = vk::QueueFlagBits::eGraphics,
                                               bool forceRequerying = false);
      bool IsSuitable(vk::QueueFlags requiredFlags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute);

      uint32_t FindMemoryTypeIndex(uint32_t typeFilter,
                                   vk::MemoryPropertyFlags propertyFlags) const;

      Device CreateDevice(vk::PhysicalDeviceFeatures features = {});

      operator vk::PhysicalDevice() const;
      operator VkPhysicalDevice() const;

    };
  }
}
