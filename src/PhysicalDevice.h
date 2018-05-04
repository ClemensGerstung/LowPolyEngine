#pragma once

#include <vulkan/vulkan.hpp>
#include "Instance.h"

namespace lpe
{
  namespace vulkan
  {
    class PhysicalDevice
    {
    private:
      vk::PhysicalDeviceProperties properties;
      vk::PhysicalDeviceFeatures feature;
      vk::PhysicalDeviceMemoryProperties memoryProperties;
      
      vk::PhysicalDevice physicalDevice;
      std::unique_ptr<Instance> instance;

    public:
      PhysicalDevice() = default;
      PhysicalDevice(const PhysicalDevice& other);
      PhysicalDevice(PhysicalDevice&& other);
      PhysicalDevice& operator=(const PhysicalDevice& other);
      PhysicalDevice& operator=(PhysicalDevice&& other);
      PhysicalDevice(Instance* instance, vk::PhysicalDevice physicalDevice)
      {
        this->instance.reset(instance);
        this->physicalDevice = physicalDevice;
        this->properties = this->physicalDevice.getProperties();
        this->feature = this->physicalDevice.getFeatures();
        this->memoryProperties = this->physicalDevice.getMemoryProperties();
      }
      
      ~PhysicalDevice();

      bool IsSuitable()
      {


        return true;
      }
    };
  }
}
