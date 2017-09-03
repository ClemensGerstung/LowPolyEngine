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


