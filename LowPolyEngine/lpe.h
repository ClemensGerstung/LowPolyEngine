#ifndef LPE_H
#define LPE_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "stdafx.h"
#include <cstdint>

BEGIN_LPE

static struct 
{
  bool EnableValidationLayer = false;
} settings;

VULKAN_HPP_INLINE LPE uint32_t GetPhysicalDeviceCount()
{
	vk::ApplicationInfo appInfo = { "", 0, "", 0, VK_MAKE_VERSION(1, 0, 39) };
	vk::InstanceCreateInfo info = { {}, &appInfo };
	vk::Instance instance;
	uint32_t count = -1;

	auto result = vk::createInstance(&info, nullptr, &instance);
	helper::ThrowIfNotSuccess(result, "Couldn't get physical device count");

	result = instance.enumeratePhysicalDevices(&count, nullptr);
	helper::ThrowIfNotSuccess(result, "Couldn't get physical device count");

	instance.destroy();

	return count;
}

END_LPE

#endif