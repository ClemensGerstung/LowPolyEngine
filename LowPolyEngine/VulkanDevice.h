#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vector>
#include <vulkan/vulkan.hpp>
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

namespace lpe
{
	class Device
	{
	private:
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;

		std::shared_ptr<vk::SurfaceKHR> surface;

		QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device) const;
		bool IsDeviceSuitable(vk::PhysicalDevice device) const;
		bool CheckDeviceExtensionSupport(vk::PhysicalDevice device) const;
		SwapChainSupportDetails QuerySwapChainDetails(vk::PhysicalDevice device) const;

		void PickPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices);
		void CreateLogicalDevice();
	public:
		Device() = delete;
		Device(const Device&);
		Device operator=(const Device&) const;

		Device(Device&&) = delete;
		Device operator=(Device&&) = delete;

		Device(const vk::SurfaceKHR* surface, const std::vector<vk::PhysicalDevice>& devices);

		~Device();

		explicit operator vk::Device() const;

		const vk::Device& GetVulkanDevice() const;
		const vk::Queue& GetGraphicsQueue() const;
		const vk::Queue& GetPresentQueue() const;
	};
}

#endif