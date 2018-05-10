#pragma once

#include <vulkan/vulkan.hpp>
#include "Window.h"
#include "stdafx.h"
#include "helper.h"
#include <iostream>

namespace lpe
{
  namespace vulkan
  {
    class PhysicalDevice;

    class Instance
    {
    private:
      vk::Instance instance;
      std::vector<vk::PhysicalDevice> physicalDevices;
      VkDebugReportCallbackEXT debugReportCallback;

      VKAPI_ATTR static VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,
                                                          VkDebugReportObjectTypeEXT objType,
                                                          uint64_t obj,
                                                          size_t location,
                                                          int32_t code,
                                                          const char* layerPrefix,
                                                          const char* msg,
                                                          void* userData);

    public:
      Instance() = default;
      Instance(const Instance& other);
      Instance(Instance&& other);
      Instance& operator=(const Instance& other);
      Instance& operator=(Instance&& other);
      Instance(const std::string&& name,
               const uint32_t version);
      ~Instance();

      bool CheckValidationLayerSupport() const;

      PhysicalDevice GetSuitablePhysicalDevice(vk::SurfaceKHR surface,
                                               bool forceRequeryingPhysicalDevices = false);
      std::vector<PhysicalDevice> GetPhysicalDevices(vk::SurfaceKHR surface,
                                                     bool forceRequeryingPhysicalDevices = false);

      operator VkInstance() const;
      operator vk::Instance() const;
    };
  }
}
