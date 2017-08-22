#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vector>
#include <vulkan/vulkan.hpp>
#include "QueueFamilyIndices.h"

namespace lpe
{
	class Device
	{
	private:
		vk::PhysicalDevice physicalDevice;

		QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device) const;
		bool IsDeviceSuitable(vk::PhysicalDevice device) const;
	public:
		Device() = delete;
		Device(const Device&) = delete;
		Device(Device&&) = delete;
		Device operator=(const Device&) = delete;
		Device operator=(Device&&) = delete;

		Device(const std::vector<vk::PhysicalDevice>& devices);

		~Device();

		explicit operator vk::Device() const;
	};
}

#endif