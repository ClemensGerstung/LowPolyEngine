#ifndef LPE_H
#define LPE_H
#include <vulkan/vulkan.hpp>

namespace lpe
{
	inline uint32_t GetPhysicalDeviceCount()
	{
		vk::ApplicationInfo appInfo = {"", 0, "", 0, VK_MAKE_VERSION(1, 0, 39)};
		vk::InstanceCreateInfo info = { {}, &appInfo };
		vk::Instance instance;
		uint32_t count = -1;

		auto result = vk::createInstance(&info, nullptr, &instance);
		if(result != vk::Result::eSuccess)
		{
			throw std::runtime_error("Couldn't get physical device count (Error " + vk::to_string(result) +")");
		}

		result = instance.enumeratePhysicalDevices(&count, nullptr);
		if (result != vk::Result::eSuccess)
		{
			throw std::runtime_error("Couldn't get physical device count (Error " + vk::to_string(result) + ")");
		}

		instance.destroy();

		return count;
	}
}

#endif