#pragma once

#include <set>
#include <vulkan/vulkan.hpp>
#include "Instance.h"

namespace lpe
{
  namespace vulkan
  {
    struct QueueFamilyIndices
    {
      int32_t graphicsFamily = -1;
      int32_t presentFamily = -1;
      int32_t computeFamily = -1;

      bool IsComplete() const;
      std::array<int32_t, 2> ToArray() const;
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
      std::unique_ptr<lpe::vulkan::Instance, helper::Deleter<lpe::vulkan::Instance>> instance;
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

      ~PhysicalDevice() = default;

      std::vector<vk::ExtensionProperties> GetDeviceExtensions(bool forceRequerying = false);
      bool SupportsExtensions(bool forceRequerying = false);
      SwapChainSupportDetails QuerySwapChainSupportDetails(bool forceRequerying = false);
      QueueFamilyIndices GetQueueFamilyIndices(vk::QueueFlags requiredFlags = vk::QueueFlagBits::eGraphics,
                                               bool forceRequerying = false);
      bool IsSuitable(vk::QueueFlags requiredFlags = vk::QueueFlagBits::eGraphics);

      uint32_t FindMemoryTypeIndex(uint32_t typeFilter,
                                   vk::MemoryPropertyFlags propertyFlags) const;
    };
  }
}
