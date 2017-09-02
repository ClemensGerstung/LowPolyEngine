#ifndef BUFFER_H
#define BUFFER_H
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include "SwapChain.h"

namespace lpe
{
	class Buffer
	{
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		lpe::SwapChain swapChain;

	protected:
		void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);
		void CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

		uint32_t FindMemoryTypeIndex(uint32_t typeFilter, vk::MemoryPropertyFlags props) const;

	public:
		Buffer(const SwapChain& swapChain, const vk::CommandPool& commandPool, void* data, vk::DeviceSize size);
		~Buffer();
	};
}

#endif