#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include "SwapChainSupportDetails.h"
#include "QueueFamilyIndices.h"

namespace lpe {
	class  Window;

	class SwapChain
	{
	private:
		vk::Instance instance;
		vk::PhysicalDevice physicalDevice = nullptr;
		vk::Device logicalDevice;
		vk::SurfaceKHR surface;
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> swapchainImages;
		vk::Format swapchainImageFormat;
		vk::Extent2D swapchainExtent;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		vk::DebugReportCallbackEXT callback;

		QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device) const;
		bool CheckDeviceExtensionSupport(vk::PhysicalDevice device) const;
		SwapChainSupportDetails QuerySwapChainDetails(vk::PhysicalDevice device) const;
		bool IsDeviceSuitable(vk::PhysicalDevice device) const;

		void PickPhysicalDevice(const uint32_t physicalDeviceIndex);
		void CreateLogicalDevice();

		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const;
		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes) const;
		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const uint32_t width, const uint32_t height) const;

		void CreateSwapChain(const uint32_t physicalDeviceIndex, const vk::Instance& instance, const uint32_t width, const uint32_t height);

	public:
		SwapChain() = default;

		//SwapChain(const SwapChain&);
		//SwapChain(SwapChain&&) noexcept;
		//SwapChain operator =(const SwapChain&) const;
		//SwapChain operator =(SwapChain&&) const noexcept;

		SwapChain(std::string appName, const lpe::Window* window);
		SwapChain(std::string appName, const uint32_t width, const uint32_t height);
		SwapChain(const uint32_t physicalDeviceIndex, const vk::Instance& instance, const uint32_t width, const uint32_t height);

		//~SwapChain();

		//void Recreate(const uint32_t width, const uint32_t height);
	};
}
#endif