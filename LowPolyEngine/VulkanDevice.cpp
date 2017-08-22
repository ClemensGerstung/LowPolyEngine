#include "VulkanDevice.h"

QueueFamilyIndices lpe::Device::FindQueueFamilies(vk::PhysicalDevice device) const
{
	QueueFamilyIndices indices;
	auto properties = device.getQueueFamilyProperties();

	int index = 0;

	for (const auto& prop : properties)
	{
		
	}
}

bool lpe::Device::IsDeviceSuitable(vk::PhysicalDevice device) const
{
	return false;
}

lpe::Device::Device(const std::vector<vk::PhysicalDevice>& devices)
{
	if (devices.empty())
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if ((void*)physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find suitable GPU!");
	}
}

lpe::Device::~Device()
{
	
}

lpe::Device::operator vk::Device() const
{
	return nullptr;
}

