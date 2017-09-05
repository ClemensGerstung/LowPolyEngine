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

void lpe::Commands::CreateCommandBuffers(const std::vector<vk::Framebuffer>& framebuffers,
                                         vk::Extent2D extent,
                                         const vk::DescriptorSet& descriptorSet,
                                         const vk::RenderPass& renderPass,
                                         const vk::Pipeline& graphicsPipeline,
                                         const vk::PipelineLayout& pipelineLayout,
                                         const Model& model,
                                         const ModelRenderer& renderer)
{
	commandBuffers.resize(framebuffers.size());

	vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, (uint32_t)commandBuffers.size() };

	auto result = device.allocateCommandBuffers(&allocInfo, commandBuffers.data());
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to allocate command buffers! (" + vk::to_string(result) + ")");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) 
	{
		vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eSimultaneousUse };

		commandBuffers[i].begin(&beginInfo);

		std::array<vk::ClearValue, 2> clearValues = {};
		clearValues[0].color = { std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 1.0f }) };
		clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

		vk::RenderPassBeginInfo renderPassInfo = { renderPass, framebuffers[i], { {0, 0}, extent }, (uint32_t)clearValues.size(), clearValues.data() };

		commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		vk::Buffer vertexBuffers[] = { renderer.GetVertexBuffer() };
		vk::DeviceSize offsets[] = { 0 };
		commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);

		commandBuffers[i].bindIndexBuffer(renderer.GetIndicesBuffer(), 0, vk::IndexType::eUint32);

		commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		commandBuffers[i].drawIndexed((uint32_t)model.GetIndices().size(), 1, 0, 0, 0);

		commandBuffers[i].endRenderPass();

		commandBuffers[i].end();
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

std::vector<vk::CommandBuffer> lpe::Commands::GetCommandBuffers() const
{
	return commandBuffers;
}
