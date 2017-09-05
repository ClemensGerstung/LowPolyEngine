#ifndef COMMANDS_H
#define COMMANDS_H
#include <vulkan/vulkan.hpp>
#include "ModelRenderer.h"
#include "GraphicsPipeline.h"
#include "SwapChain.h"

namespace lpe
{
	class Commands
	{
		vk::Device device;
		vk::CommandPool commandPool;
		std::vector<vk::CommandBuffer> commandBuffers;
		std::unique_ptr<vk::DescriptorSet> descriptorSet;

	public:
		Commands() = default;
		~Commands();

		void CreateCommandPool(const vk::Device& device, uint32_t graphicsFamilyIndex);
		void CreateCommandBuffers(const SwapChain& swapChain, const GraphicsPipeline& pipeline, const Model& model, const ModelRenderer& renderer);

		vk::CommandBuffer BeginSingleTimeCommands() const;
		void EndSingleTimeCommands(vk::CommandBuffer commandBuffer, const vk::Queue& queue);
	};
}

#endif