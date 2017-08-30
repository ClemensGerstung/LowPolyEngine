#ifndef LPE_H
#define LPE_H
#include <vulkan/vulkan.hpp>

#include "stdafx.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace lpe
{
/**
 * \brief Gets the count of Vulkan capable graphicscards
 * \author Clemens Gerstung
 * \throws std::runtime_error if any operation failed during execution. Exception message will contain the necessary informations
 * \return number of capable card if operation was successful, otherwise -1
 */
inline uint32_t GetPhysicalDeviceCount()
{
	vk::ApplicationInfo appInfo = {"", 0, "", 0, VK_MAKE_VERSION(1, 0, 39)};
	vk::InstanceCreateInfo info = {{}, &appInfo};
	vk::Instance instance;
	uint32_t count = -1;

	auto result = vk::createInstance(&info, nullptr, &instance);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Couldn't get physical device count (Error " + vk::to_string(result) + ")");
	}

	result = instance.enumeratePhysicalDevices(&count, nullptr);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Couldn't get physical device count (Error " + vk::to_string(result) + ")");
	}

	instance.destroy();

	return count;
}

inline vk::Instance CreateInstance(const std::string& applicationName,
                                   const vk::AllocationCallbacks* allocator = nullptr,
                                   vk::DebugReportCallbackEXT* callback = nullptr)
{
#ifdef ENABLE_VALIDATION_LAYER
	if (!helper::CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available");
	}
#endif

	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++)
	{
		extensions.push_back(glfwExtensions[i]);
	}

#ifdef ENABLE_VALIDATION_LAYER
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

	vk::ApplicationInfo appInfo =
	{
		applicationName.c_str(),
		APPLICATION_VERSION,
		ENGINE_NAME,
		ENGINE_VERSION,
		VK_MAKE_VERSION(1, 0, 39)
	};

	vk::InstanceCreateInfo createInfo =
	{
		{},
		&appInfo,
#ifdef ENABLE_VALIDATION_LAYER
		(uint32_t)helper::ValidationLayer.size(),
		helper::ValidationLayer.data(),
#else
		0,
		nullptr,
#endif
		(uint32_t)extensions.size(),
		extensions.data()
	};

	vk::Instance instance;

	auto result = vk::createInstance(&createInfo, allocator, &instance);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Couldn't create Vulkan instance (Error: " + vk::to_string(result) + ")");
	}

#ifdef ENABLE_VALIDATION_LAYER
	if (callback != nullptr) {
		VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
		debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		debugReportCreateInfo.pfnCallback = helper::debugCallback;

		if (helper::CreateDebugReportCallbackEXT(instance, &debugReportCreateInfo, nullptr, callback))
		{
			throw std::runtime_error("Failed to set up debug callback");
		}
	}
#endif

	return instance;
}

}

#endif
