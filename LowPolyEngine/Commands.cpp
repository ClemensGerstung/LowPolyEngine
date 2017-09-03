#include "Commands.h"

lpe::Commands::~Commands()
{
}

void lpe::Commands::CreateCommandPool(const vk::Device& device, uint32_t graphicsFamilyIndex)
{
	this->device = device;

	vk::CommandPoolCreateInfo createInfo = { {}, graphicsFamilyIndex };

	auto result = this->device.createCommandPool(&createInfo, nullptr, &commandPool);

	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create graphics command pool! (" + vk::to_string(result) + ")");
	}
}

vk::CommandBuffer lpe::Commands::BeginSingleTimeCommands() const
{
	vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, 1 };
	vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo)[0];

	vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void lpe::Commands::EndSingleTimeCommands(vk::CommandBuffer commandBuffer, const vk::Queue& queue)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo = { 0, nullptr, nullptr, 1, &commandBuffer };

	queue.submit(1, &submitInfo, nullptr);
	queue.waitIdle();

	device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}
