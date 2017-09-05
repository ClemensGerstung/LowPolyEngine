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
		friend class Texture;
		friend class ModelRenderer;

		vk::Buffer buffer;
		vk::DeviceMemory memory;

	protected:
		void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);
		void CopyBuffer(lpe::Commands& commands, const vk::Queue& graphicsQueue, vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

	public:
		Buffer() = default;
		//Buffer(const Buffer& buffer);
		//Buffer(Buffer&& buffer);
		Buffer(vk::PhysicalDevice physicalDevice, const vk::Device& device);
		~Buffer();

		void Create(lpe::Commands& commands, const vk::Queue& graphicsQueue, void* data, vk::DeviceSize size);

		operator vk::Buffer() const;
	};
}

#endif