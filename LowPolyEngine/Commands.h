#ifndef COMMANDS_H
#define COMMANDS_H
#include <vulkan/vulkan.hpp>

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
	};
}

#endif