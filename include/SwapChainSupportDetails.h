#ifndef SWAPCHAINSUPPORTDETAILS_H
#define SWAPCHAINSUPPORTDETAILS_H

#include <vulkan/vulkan.hpp>
#include <vector>

struct SwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

#endif