#ifndef COMMANDS_H
#define COMMANDS_H
#include <vulkan/vulkan.hpp>
#include "ModelRenderer.h"

namespace lpe
{
	class Commands
	{
		vk::Device device;
		vk::CommandPool commandPool;
		std::vector<vk::CommandBuffer> commandBuffers;

	public:
		Commands() = default;
		~Commands();

		void CreateCommandPool(const vk::Device& device, uint32_t graphicsFamilyIndex);
		void CreateCommandBuffers(const std::vector<vk::Framebuffer>& framebuffers,
		                          vk::Extent2D extent,
		                          const vk::DescriptorSet& descriptorSet,
		                          const vk::RenderPass& renderPass,
		                          const vk::Pipeline& graphicsPipeline,
		                          const vk::PipelineLayout& pipelineLayout,
		                          const Model& model,
		                          const ModelRenderer& renderer);

		vk::CommandBuffer BeginSingleTimeCommands() const;
		void EndSingleTimeCommands(vk::CommandBuffer commandBuffer, const vk::Queue& queue);


		std::vector<vk::CommandBuffer> GetCommandBuffers() const;
	};
}

#endif