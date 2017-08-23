#include "VulkanDevice.h"
#include <set>
#include "stdafx.h"

QueueFamilyIndices lpe::Device::FindQueueFamilies(vk::PhysicalDevice device) const
{
	QueueFamilyIndices indices;
	auto queueFamilies = device.getQueueFamilyProperties();

	int index = 0;

	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = index;
		}

		vk::Bool32 presentSupport = device.getSurfaceSupportKHR(index, *surface.get());


		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = index;
		}

		if (indices.isComplete())
		{
			break;
		}

		index++;
	}

	return indices;
}

bool lpe::Device::CheckDeviceExtensionSupport(vk::PhysicalDevice device) const
{
	auto extensions = device.enumerateDeviceExtensionProperties();

	std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

	for (const auto& extension : extensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportDetails lpe::Device::QuerySwapChainDetails(vk::PhysicalDevice device) const
{
	SwapChainSupportDetails details;

	details.capabilities = device.getSurfaceCapabilitiesKHR(*surface.get());
	details.formats = device.getSurfaceFormatsKHR(*surface.get());
	details.presentModes = device.getSurfacePresentModesKHR(*surface.get());

	return details;
}

bool lpe::Device::IsDeviceSuitable(vk::PhysicalDevice device) const
{
	auto indices = FindQueueFamilies(device);

	bool supportsExtensions = CheckDeviceExtensionSupport(device);

	bool isSwapChainAdequate = false;
	if(supportsExtensions)
	{
		auto swapChainDetails = QuerySwapChainDetails(device);
		isSwapChainAdequate = !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
	}

	return indices.isComplete() && supportsExtensions && isSwapChainAdequate;
}

void lpe::Device::PickPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices)
{
	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == vk::PhysicalDevice{})
	{
		throw std::runtime_error("failed to find suitable GPU!");
	}
}

void lpe::Device::CreateLogicalDevice()
{
	auto indices = FindQueueFamilies(physicalDevice);
	float queuePriority = 1.0f;

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	for (auto queueFamily : {indices.graphicsFamily, indices.presentFamily})
	{
		queueCreateInfos.push_back({ {}, queueFamily, 1, &queuePriority });
	}

	vk::DeviceCreateInfo createInfo =
	{
		{},
		(uint32_t)queueCreateInfos.size(),
		queueCreateInfos.data(),
		(uint32_t)(EnableValidationLayer ? ValidationLayer.size() : 0),
		(EnableValidationLayer ? ValidationLayer.data() : nullptr),
		(uint32_t)DeviceExtensions.size(),
		DeviceExtensions.data(),
		{}
	};

	device = physicalDevice.createDevice(createInfo, nullptr);

	graphicsQueue = device.getQueue(indices.graphicsFamily, 0);
	presentQueue = device.getQueue(indices.presentFamily, 0);
}

void lpe::Device::ThrowIfDefaultInit() const
{
	if (!isDefaultCreated)
		throw std::runtime_error("Device not initialized correctly");
}

lpe::Device::Device()
{
	isDefaultCreated = true;
}

lpe::Device::Device(const Device& device)
	: isDefaultCreated(false),
	  physicalDevice(device.physicalDevice),
	  device(device.device),
	  graphicsQueue(device.graphicsQueue),
	  presentQueue(device.presentQueue)
{
	
}

lpe::Device lpe::Device::operator=(const Device& device) const
{
	return { device };
}

lpe::Device::Device(const vk::SurfaceKHR* surface, const std::vector<vk::PhysicalDevice>& devices)
	: isDefaultCreated(false)
{
	if (devices.empty())
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	this->surface.reset(const_cast<vk::SurfaceKHR*>(surface));

	PickPhysicalDevice(devices);

	CreateLogicalDevice();
}

lpe::Device::~Device()
{
	if (!isDefaultCreated)
	{
		device.destroy(nullptr);
	}
}

lpe::Device::operator vk::Device() const
{
	ThrowIfDefaultInit();

	return device;
}

const vk::Device& lpe::Device::GetVulkanDevice() const
{
	ThrowIfDefaultInit();

	return device;
}

const vk::Queue& lpe::Device::GetGraphicsQueue() const
{
	ThrowIfDefaultInit();

	return graphicsQueue;
}

const vk::Queue& lpe::Device::GetPresentQueue() const
{
	ThrowIfDefaultInit();

	return presentQueue;
}

