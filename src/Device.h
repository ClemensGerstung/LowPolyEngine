#pragma once

#include "helper.h"

#include <map>

namespace lpe
{
  namespace vulkan
  {
    class Instance;
    class PhysicalDevice;
    class SwapChain;

    class Device
    {
    private:
      helper::Pointer<Instance> instance;
      helper::Pointer<PhysicalDevice> physicalDevice;
      vk::Device device;
      vk::SurfaceKHR surface;

      /**
       * \brief queuefamily -> queue
       */
      std::map<uint32_t, vk::Queue> queues;

    public:
      Device() = default;
      Device(const Device& other);
      Device(Device&& other) noexcept;
      Device& operator=(const Device& other);
      Device& operator=(Device&& other) noexcept;
      Device(Instance* instance,
             PhysicalDevice* physicalDevice,
             vk::PhysicalDevice vulkanPhysicalDevice,
             vk::SurfaceKHR surface,
             vk::PhysicalDeviceFeatures features);

      ~Device();

      vk::Queue GetQueue(uint32_t familyIndex);
      SwapChain CreateSwapChain(vk::PresentModeKHR presentMode = vk::PresentModeKHR::eMailbox,
                                vk::Format format = vk::Format::eB8G8R8A8Unorm,
                                vk::ColorSpaceKHR colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear);

      operator vk::Device() const;
      operator VkDevice() const;
    };
  }
}
