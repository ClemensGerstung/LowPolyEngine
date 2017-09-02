#include "Buffer.h"

void lpe::Buffer::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory)
{
	vk::BufferCreateInfo createInfo = {{}, size, usage, vk::SharingMode::eExclusive};

	auto logicalDevice = swapChain.GetLogicalDevice();
	auto result = logicalDevice.createBuffer(&createInfo, nullptr, &buffer);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create buffer!" + vk::to_string(result) + "");
	}

	vk::MemoryRequirements requirements = logicalDevice.getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocInfo = { requirements.size, FindMemoryTypeIndex(requirements.memoryTypeBits, properties) };

	result = logicalDevice.allocateMemory(&allocInfo, nullptr, &memory);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to allocate buffer memory!" + vk::to_string(result) + "");
	}

	logicalDevice.bindBufferMemory(buffer, memory, 0);
}

void lpe::Buffer::CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
{
	vk::CommandBuffer commandBuffer = swapChain.BeginSingleTimeCommands();

	vk::BufferCopy copyRegion = {0, 0, size};

	commandBuffer.copyBuffer(src, dst, 1, &copyRegion);

	swapChain.EndSingleTimeCommands(commandBuffer);
}

uint32_t lpe::Buffer::FindMemoryTypeIndex(uint32_t typeFilter, vk::MemoryPropertyFlags props) const
{
	auto properties = swapChain.GetPhysicalDevice().getMemoryProperties();

	for(uint32_t i = 0; i < properties.memoryTypeCount; i++)
	{
		if((typeFilter & (1 << i)) && (properties.memoryTypes[i].propertyFlags & props) == props)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

lpe::Buffer::Buffer(const SwapChain& swapChain, const vk::CommandPool& commandPool, void* data, vk::DeviceSize size)
{
	this->swapChain = swapChain;

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingMemory;
	auto logicalDevice = swapChain.GetLogicalDevice();

	CreateBuffer(size, 
				 vk::BufferUsageFlagBits::eTransferSrc, 
				 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
				 stagingBuffer, 
				 stagingMemory);

	void* stagingData;
	
	logicalDevice.mapMemory(stagingMemory, 0, size, {}, &stagingData);
	memcpy(stagingData, data, (size_t)size);
	logicalDevice.unmapMemory(stagingMemory);

	CreateBuffer(size,
				 vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
				 vk::MemoryPropertyFlagBits::eDeviceLocal,
				 buffer,
				 memory);

	CopyBuffer(stagingBuffer, buffer, size);

	logicalDevice.destroyBuffer(stagingBuffer);
	logicalDevice.freeMemory(stagingMemory);
}

lpe::Buffer::~Buffer()
{
	auto logicalDevice = swapChain.GetLogicalDevice();

	logicalDevice.destroyBuffer(buffer);
	logicalDevice.freeMemory(memory);
}
