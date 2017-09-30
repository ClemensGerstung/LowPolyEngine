#ifndef DEVICE_H
#define DEVICE_H

#include <vulkan/vulkan.hpp>
#include <assert.h>

namespace lpe
{
  VULKAN_HPP_INLINE vk::QueueFlagBits operator&(const vk::QueueFlagBits& lhs, const vk::QueueFlagBits& rhs)
  {
    return (vk::QueueFlagBits)((uint32_t)lhs & (uint32_t)rhs);
  }

  VULKAN_HPP_INLINE bool operator==(const vk::QueueFlags& lhs, const int& rhs)
  {
    return (uint32_t)lhs == (uint32_t)rhs;
  }

  struct VulkanDevice
  {
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::PhysicalDeviceProperties properties;
    vk::PhysicalDeviceFeatures features;
    vk::PhysicalDeviceFeatures enabledFeatures;
    vk::PhysicalDeviceMemoryProperties memoryProperties;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
    std::vector<std::string> supportedExtensions;
    vk::CommandPool commandPool = nullptr;
    bool enableDebugMarkers = false;

    struct
    {
      uint32_t graphics;
      uint32_t compute;
      uint32_t transfer;
    } queueFamilyIndices;

    operator VkDevice() { return (VkDevice)device; }
    operator vk::Device() { return device; }

    VulkanDevice(vk::PhysicalDevice physicalDevice)
    {
      assert(physicalDevice);
      this->physicalDevice = physicalDevice;
      properties = physicalDevice.getProperties();
      features = physicalDevice.getFeatures();
      memoryProperties = physicalDevice.getMemoryProperties();
      queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

      auto extensions = physicalDevice.enumerateDeviceExtensionProperties();
      for(const auto& extension : extensions)
      {
        supportedExtensions.push_back(extension.extensionName);
      }
    }

    ~VulkanDevice()
    {
      if(device)
      {
        if (commandPool)
        {
          device.destroyCommandPool(commandPool, nullptr);
        }

        device.destroy();
      }
    }

    uint32_t GetMemoryType(uint32_t typeBits, vk::MemoryPropertyFlagBits props, VkBool32* memTypeFound = nullptr)
    {
      for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
      {
        if ((typeBits & 1) == 1)
        {
          if ((memoryProperties.memoryTypes[i].propertyFlags & props) == props)
          {
            if (memTypeFound)
            {
              *memTypeFound = true;
            }

            return i;
          }
        }

        typeBits >>= 1;
      }

      if (memTypeFound)
      {
        *memTypeFound = false;
        return 0;
      }

      throw std::runtime_error("Could not find a matching memory type");
    }

    uint32_t GetQueueFamilyIndex(vk::QueueFlagBits queueFlags)
    {
      // Dedicated queue for compute
      // Try to find a queue family index that supports compute but not graphics
      if ((queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
      {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
          if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) == 0))
          {
            return i;
            break;
          }
        }
      }

      // Dedicated queue for transfer
      // Try to find a queue family index that supports transfer but not graphics and compute
      if ((queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
      {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
          if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) == 0) && ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) == 0))
          {
            return i;
            break;
          }
        }
      }

      // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
      for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
      {
        if (queueFamilyProperties[i].queueFlags & queueFlags)
        {
          return i;
          break;
        }
      }

      throw std::runtime_error("Could not find a matching queue family index");
    }

    vk::Result CreateLogicalDevice(vk::PhysicalDeviceFeatures enabledFeatures, std::vector<std::string> enabledExtensions, bool useSwapChain = true, vk::QueueFlags requestedQueueTypes = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)
    {
      std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = { };

      const float defaultQueuePriority(0.0f);

      if (requestedQueueTypes & vk::QueueFlagBits::eGraphics)
      {
        queueFamilyIndices.graphics = GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics);
        vk::DeviceQueueCreateInfo queueInfo = { {}, queueFamilyIndices.graphics, 1, &defaultQueuePriority };
        queueCreateInfos.push_back(queueInfo);
      }
      else
      {
        queueFamilyIndices.graphics = VK_NULL_HANDLE;
      }

      if (requestedQueueTypes & vk::QueueFlagBits::eCompute)
      {
        queueFamilyIndices.compute = GetQueueFamilyIndex(vk::QueueFlagBits::eCompute);
        if (queueFamilyIndices.compute != queueFamilyIndices.graphics)
        {
          vk::DeviceQueueCreateInfo queueInfo = { {}, queueFamilyIndices.compute, 1, &defaultQueuePriority };
          queueCreateInfos.push_back(queueInfo);
        }
      }
      else
      {
        queueFamilyIndices.compute = queueFamilyIndices.graphics;
      }

      if (requestedQueueTypes & vk::QueueFlagBits::eTransfer)
      {
        queueFamilyIndices.transfer = GetQueueFamilyIndex(vk::QueueFlagBits::eTransfer);
        if ((queueFamilyIndices.transfer != queueFamilyIndices.graphics) && (queueFamilyIndices.transfer != queueFamilyIndices.compute))
        {
          vk::DeviceQueueCreateInfo queueInfo = { {}, queueFamilyIndices.transfer, 1, &defaultQueuePriority };
          queueCreateInfos.push_back(queueInfo);
        }
      }
      else
      {
        queueFamilyIndices.transfer = queueFamilyIndices.graphics;
      }

      std::vector<std::string> deviceExtensions(enabledExtensions);

      if(useSwapChain)
      {
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
      }

      vk::DeviceCreateInfo deviceCreateInfo = { {}, static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data() };
      deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

    }

    bool extensionSupported(std::string extension)
    {
      return (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end());
    }
  };
}

#endif