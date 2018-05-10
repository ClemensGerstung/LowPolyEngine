#pragma once

#include "helper.h"

namespace lpe
{
  namespace vulkan
  {
    class Instance;
    class PhysicalDevice;

    class Device
    {
    private:
      helper::Pointer<Instance> instance;
      helper::Pointer<PhysicalDevice> physicalDevice;
      vk::Device device;

    public:
      Device() = default;
      Device(const Device& other);
      Device(Device&& other) noexcept;
      Device& operator=(const Device& other);
      Device& operator=(Device&& other) noexcept;
      Device(Instance* instance,
             PhysicalDevice* physicalDevice,
             vk::PhysicalDevice vulkanPhysicalDevice,
             vk::PhysicalDeviceFeatures features);

      ~Device();
    };
  }
}
