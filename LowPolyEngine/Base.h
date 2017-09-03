#ifndef BASE_H
#define BASE_H
#include <vulkan/vulkan.hpp>

namespace lpe
{
	// TODO: rename
	class Base
	{
	protected:
		vk::Device device;
		vk::PhysicalDevice physicalDevice;

		uint32_t FindMemoryTypeIndex(uint32_t typeFilter, vk::MemoryPropertyFlags props) const;

	public:
		Base() = default;
		Base(vk::PhysicalDevice physicalDevice, const vk::Device& device);

		virtual ~Base() = default;
	};
}

#endif
