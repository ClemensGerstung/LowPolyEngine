#include "Buffer.h"
#include "Commands.h"

void lpe::Buffer::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory)
{
	vk::BufferCreateInfo createInfo = {{}, size, usage, vk::SharingMode::eExclusive};

	auto result = device.createBuffer(&createInfo, nullptr, &buffer);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create buffer!" + vk::to_string(result) + "");
	}

	vk::MemoryRequirements requirements = device.getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocInfo = { requirements.size, FindMemoryTypeIndex(requirements.memoryTypeBits, properties) };

	result = device.allocateMemory(&allocInfo, nullptr, &memory);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to allocate buffer memory!" + vk::to_string(result) + "");
	}

	device.bindBufferMemory(buffer, memory, 0);
}

void lpe::Buffer::CopyBuffer(lpe::Commands& commands, vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
{
	vk::CommandBuffer commandBuffer = commands.BeginSingleTimeCommands();

	vk::BufferCopy copyRegion = {0, 0, size};

	commandBuffer.copyBuffer(src, dst, 1, &copyRegion);

	commands.EndSingleTimeCommands(commandBuffer, graphicsQueue);
}

lpe::Buffer::Buffer(vk::PhysicalDevice physicalDevice, const vk::Device& device, const vk::Queue& graphicsQueue)
	: Base(physicalDevice, device)
{
	this->graphicsQueue = graphicsQueue;
}

void lpe::Buffer::Create(lpe::Commands& commands, void* data, vk::DeviceSize size)
{
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingMemory;

	CreateBuffer(size, 
				 vk::BufferUsageFlagBits::eTransferSrc, 
				 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
				 stagingBuffer, 
				 stagingMemory);

	void* stagingData;
	
	device.mapMemory(stagingMemory, 0, size, {}, &stagingData);
	memcpy(stagingData, data, (size_t)size);
	device.unmapMemory(stagingMemory);

	CreateBuffer(size,
				 vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
				 vk::MemoryPropertyFlagBits::eDeviceLocal,
				 buffer,
				 memory);

	CopyBuffer(commands, stagingBuffer, buffer, size);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingMemory);
}

lpe::Buffer::~Buffer()
{
	device.destroyBuffer(buffer);
	device.freeMemory(memory);
}
