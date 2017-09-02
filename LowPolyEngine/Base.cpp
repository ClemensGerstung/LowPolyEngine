#include "Base.h"



uint32_t lpe::Base::FindMemoryTypeIndex(uint32_t typeFilter, vk::MemoryPropertyFlags props) const
{
	auto properties = physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (properties.memoryTypes[i].propertyFlags & props) == props)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

lpe::Base::Base(vk::PhysicalDevice physicalDevice, const vk::Device& device)
{
	this->physicalDevice = physicalDevice;
	this->device = device;
}
