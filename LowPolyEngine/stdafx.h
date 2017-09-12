#ifndef STDAFX_H
#define STDAFX_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

// may be overridden by the client code
#define APPLICATION_VERSION 1

#define BEGIN_LPE namespace lpe {
#define END_LPE }

#define LPE // this is a simple static lib but if somebody want to turn this into a dynamic lib consider defining this as "__declspec" or "__attribute__"!
// also only needed if any (namespace) standalone methods...

BEGIN_LPE
	namespace helper
	{
#define ENGINE_NAME "LowPolyEngine"
#define ENGINE_VERSION VK_MAKE_VERSION(0, 0, 1)

#define ENABLE_VALIDATION_LAYER true

		VULKAN_HPP_INLINE LPE void ThrowIfNotSuccess(vk::Result result, std::string message)
		{
			if (result != vk::Result::eSuccess)
			{
				throw std::runtime_error(message + " (Result: " + vk::to_string(result) + ")");
			}
		}

		const std::vector<const char*> ValidationLayer = {
			"VK_LAYER_LUNARG_core_validation"
		};

		const std::vector<const char*> DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

    VULKAN_HPP_INLINE LPE uint32_t FindMemoryTypeIndex(uint32_t typeFilter, vk::MemoryPropertyFlags property, const vk::PhysicalDeviceMemoryProperties& properties)
    {
      for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
      {
        if ((typeFilter & (1 << i)) && (properties.memoryTypes[i].propertyFlags & property) == property)
        {
          return i;
        }
      }

      throw std::runtime_error("failed to find suitable memory type!");
    }

		VULKAN_HPP_INLINE LPE bool CheckValidationLayerSupport()
		{
			auto availableLayers = vk::enumerateInstanceLayerProperties();

			for (const auto& layerName : ValidationLayer)
			{
				bool found = false;

				for (const auto& layerProp : availableLayers)
				{
					if (strcmp(layerName, layerProp.layerName) == 0)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					return false;
				}
			}

			return true;
		}

		VULKAN_HPP_INLINE LPE VkResult CreateDebugReportCallbackEXT(vk::Instance instance,
		                                                            const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
		                                                            const VkAllocationCallbacks* pAllocator,
		                                                            vk::DebugReportCallbackEXT* pCallback)
		{
			auto func = (PFN_vkCreateDebugReportCallbackEXT)instance.getProcAddr("vkCreateDebugReportCallbackEXT");
			if (func != nullptr)
			{
				return func(static_cast<VkInstance_T*>(instance), pCreateInfo, pAllocator,
				            reinterpret_cast<VkDebugReportCallbackEXT*>(pCallback));
			}

			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		inline void DestroyDebugReportCallbackEXT(vk::Instance instance,
		                                          vk::DebugReportCallbackEXT callback,
		                                          const VkAllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugReportCallbackEXT)instance.getProcAddr("vkDestroyDebugReportCallbackEXT");
			if (func != nullptr)
			{
				func(static_cast<VkInstance>(instance), static_cast<VkDebugReportCallbackEXT>(callback), pAllocator);
			}
		}

		VULKAN_HPP_INLINE LPE VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags,
		                                                                   VkDebugReportObjectTypeEXT objType,
		                                                                   uint64_t obj,
		                                                                   size_t location,
		                                                                   int32_t code,
		                                                                   const char* layerPrefix,
		                                                                   const char* msg,
		                                                                   void* userData)
		{
			std::cerr << "validaion layer: " << msg << std::endl;

			return VK_FALSE;
		}

		VULKAN_HPP_INLINE LPE std::vector<char> ReadSPIRVFile(std::string fileName)
		{
			std::ifstream file(fileName, std::ios::ate | std::ios::binary);

			if (!file.is_open())
			{
				throw std::runtime_error("failed to open file \"" + fileName + "\"!");
			}

			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);

			file.close();

			return buffer;
		}

	  VULKAN_HPP_INLINE LPE void* AlignedAlloc(size_t size, size_t alignment)
	  {
	    void* data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
	    data = _aligned_malloc(size, alignment);
#else
      int res = posix_memalign(&data, alignment, size);
      if (res != 0)
        data = nullptr;
#endif
	    return data;
	  }

	  VULKAN_HPP_INLINE LPE void AlignedFree(void* data)
	  {
#if	defined(_MSC_VER) || defined(__MINGW32__)
	    _aligned_free(data);
#else
      free(data);
#endif
	  }
	}

END_LPE

#endif
