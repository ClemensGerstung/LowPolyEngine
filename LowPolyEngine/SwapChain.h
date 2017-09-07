#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include "SwapChainSupportDetails.h"
#include "QueueFamilyIndices.h"

#include "ImageView.h"

#include <GLFW/glfw3.h>

namespace lpe {

	class SwapChain
	{
	private:
		vk::Instance instance;
		vk::PhysicalDevice physicalDevice = nullptr;
		vk::Device device;
		vk::SurfaceKHR surface;
		vk::SwapchainKHR swapchain;
		std::vector<lpe::ImageView> imageViews;
		std::vector<vk::Framebuffer> framebuffers;
		vk::Format imageFormat;
		vk::Extent2D extent;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		vk::DebugReportCallbackEXT callback;

		vk::Semaphore imageAvailableSemaphore;
		vk::Semaphore renderAvailableSemaphore;

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

		void CreateImageViews();

	public:
		SwapChain() = default;

		~SwapChain();

		//void Recreate(const uint32_t width, const uint32_t height);

		void Init(std::string appName, GLFWwindow* window, const uint32_t width, const uint32_t height);
		void Init(std::string appName, GLFWwindow* window, const uint32_t width, const uint32_t height, const uint32_t physicalDeviceIndex);

		void CreateFrameBuffers(const lpe::ImageView& depthImage, const vk::RenderPass& renderPass);

		void CreateSemaphores();

		void DrawFrame(const std::vector<vk::CommandBuffer>& commandBuffers);

		QueueFamilyIndices FindQueueFamilies() const;

		vk::Device GetLogicalDevice() const;
		vk::PhysicalDevice GetPhysicalDevice() const;
		vk::Format GetSwapChainImageFormat() const;
		vk::Extent2D GetSwapChainExtent() const;
		vk::Queue GetGraphicsQueue() const;
		vk::Queue GetPresentQueue() const;
		std::vector<vk::Framebuffer> GetFramebuffers() const;

		vk::Device* GetLogicalDeviceRef();
	};
}

#endif