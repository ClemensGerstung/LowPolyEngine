#ifndef BUFFER_H
#define BUFFER_H
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include "Base.h"

namespace lpe
{
	class Commands;

	class Buffer : public Base
	{
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		vk::Queue graphicsQueue;

	protected:
		void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);
		void CopyBuffer(lpe::Commands& commands, vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);


	public:
		Buffer(vk::PhysicalDevice physicalDevice, const vk::Device& device, const vk::Queue& graphicsQueue);
		~Buffer();

		void Create(lpe::Commands& commands, void* data, vk::DeviceSize size);
	};
}

#endif