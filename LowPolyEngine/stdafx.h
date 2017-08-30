#ifndef STDAFX_H
#define STDAFX_H

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>
#include <iostream>

namespace lpe
{
namespace helper
{
#define ENGINE_NAME "LowPolyEngine"
#define ENGINE_VERSION VK_MAKE_VERSION(0, 0, 1)

// may be overridden by the client code
#define APPLICATION_VERSION 1

#define ENABLE_VALIDATION_LAYER true

const std::vector<const char*> ValidationLayer = {
	"VK_LAYER_LUNARG_core_validation"
};

const std::vector<const char*> DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

inline bool CheckValidationLayerSupport()
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

inline VkResult CreateDebugReportCallbackEXT(vk::Instance instance,
                                             const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                             const VkAllocationCallbacks* pAllocator,
                                             vk::DebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)instance.getProcAddr("vkCreateDebugReportCallbackEXT");
	if (func != nullptr)
	{
		return func(static_cast<VkInstance_T*>(instance), pCreateInfo, pAllocator, reinterpret_cast<VkDebugReportCallbackEXT*>(pCallback));
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

inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags,
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

} // end namespace helper

}

#endif
