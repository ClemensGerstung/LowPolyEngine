#ifndef BUFFER_H
#define BUFFER_H
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include "Base.h"

namespace lpe
{
	class Buffer : public Base
	{
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		vk::CommandPool commandPool;
		vk::Queue graphicsQueue;

	protected:
		void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);
		void CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

		vk::CommandBuffer BeginSingleTimeCommands() const;
		void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
	public:
		Buffer(vk::PhysicalDevice physicalDevice, const vk::Device& device, const vk::CommandPool& commandPool, const vk::Queue& graphicsQueue);
		~Buffer();

		void Create(const vk::CommandPool& commandPool, void* data, vk::DeviceSize size);
	};
}

#endif