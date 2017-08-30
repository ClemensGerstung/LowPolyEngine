#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include "SwapChainSupportDetails.h"
#include "QueueFamilyIndices.h"

namespace lpe {
	class SwapChain
	{
	private:
		vk::Instance instance;
		vk::PhysicalDevice physicalDevice;
		vk::Device logicalDevice;
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> swapchainImages;
		vk::Format swapchainImageFormat;
		vk::Extent2D swapchainExtent;

		static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
		static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const uint32_t width, const uint32_t height) const;

		void CreateSwapChain(const uint32_t physicalDeviceIndex, const vk::Instance& instance, const uint32_t width, const uint32_t height);
	public:
		SwapChain() = delete;
		SwapChain(const SwapChain&);
		SwapChain(SwapChain&&) noexcept;
		SwapChain operator =(const SwapChain&) const;
		SwapChain operator =(SwapChain&&) const noexcept;

		SwapChain(std::string appName, const uint32_t width, const uint32_t height);
		SwapChain(const uint32_t physicalDeviceIndex, const vk::Instance& instance, const uint32_t width, const uint32_t height);

		~SwapChain();

		void Recreate(const uint32_t width, const uint32_t height);
	};
}
#endif