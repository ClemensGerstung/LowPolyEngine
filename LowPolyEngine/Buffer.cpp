#include "Buffer.h"

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

void lpe::Buffer::CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

	vk::BufferCopy copyRegion = {0, 0, size};

	commandBuffer.copyBuffer(src, dst, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

vk::CommandBuffer lpe::Buffer::BeginSingleTimeCommands() const
{
	vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, 1 };
	vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo)[0];

	vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void lpe::Buffer::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo = { 0, nullptr, nullptr, 1, &commandBuffer };

	graphicsQueue.submit(1, &submitInfo, nullptr);
	graphicsQueue.waitIdle();

	device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

lpe::Buffer::Buffer(vk::PhysicalDevice physicalDevice, const vk::Device& device, const vk::CommandPool& commandPool, const vk::Queue& graphicsQueue)
	: Base(physicalDevice, device)
{
	this->commandPool = commandPool;
	this->graphicsQueue = graphicsQueue;
}

void lpe::Buffer::Create(const vk::CommandPool& commandPool, void* data, vk::DeviceSize size)
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

	CopyBuffer(stagingBuffer, buffer, size);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingMemory);
}

lpe::Buffer::~Buffer()
{
	device.destroyBuffer(buffer);
	device.freeMemory(memory);
}
