#include "SwapChain.h"
#include "lpe.h"
#include "Window.h"
#include <set>

QueueFamilyIndices lpe::SwapChain::FindQueueFamilies(vk::PhysicalDevice device) const
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

		vk::Bool32 presentSupport = device.getSurfaceSupportKHR(index, surface);


		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = index;
		}

		if (indices.IsComplete())
		{
			break;
		}

		index++;
	}

	return indices;
}

bool lpe::SwapChain::CheckDeviceExtensionSupport(vk::PhysicalDevice device) const
{
	auto extensions = device.enumerateDeviceExtensionProperties();

	std::set<std::string> requiredExtensions(helper::DeviceExtensions.begin(), helper::DeviceExtensions.end());

	for (const auto& extension : extensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportDetails lpe::SwapChain::QuerySwapChainDetails(vk::PhysicalDevice device) const
{
	SwapChainSupportDetails details;

	details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	details.formats = device.getSurfaceFormatsKHR(surface);
	details.presentModes = device.getSurfacePresentModesKHR(surface);

	return details;
}

bool lpe::SwapChain::IsDeviceSuitable(vk::PhysicalDevice device) const
{
	auto indices = FindQueueFamilies(device);

	bool supportsExtensions = CheckDeviceExtensionSupport(device);

	bool isSwapChainAdequate = false;
	if (supportsExtensions)
	{
		auto swapChainDetails = QuerySwapChainDetails(device);
		isSwapChainAdequate = !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
	}

	return indices.IsComplete() && supportsExtensions && isSwapChainAdequate;
}

void lpe::SwapChain::PickPhysicalDevice(const uint32_t physicalDeviceIndex)
{
	auto physicalDevices = instance.enumeratePhysicalDevices();
	bool hasDevice = false;

	if (physicalDeviceIndex == -1)
	{
		for (const auto& device : physicalDevices)
		{
			if (IsDeviceSuitable(device))
			{
				physicalDevice = device;
				hasDevice = true;
				break;
			}
		}
	}
	else
	{
		if (IsDeviceSuitable(physicalDevices[physicalDeviceIndex]))
		{
			physicalDevice = physicalDevices[physicalDeviceIndex];
			hasDevice = true;
		}
	}

	if (!hasDevice)
	{
		throw std::runtime_error("failed to find suitable GPU!");
	}
}

void lpe::SwapChain::CreateLogicalDevice()
{
	auto indices = FindQueueFamilies(physicalDevice);
	float queuePriority = 1.0f;

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	for (auto queueFamily : {indices.graphicsFamily, indices.presentFamily})
	{
		queueCreateInfos.push_back({{}, queueFamily, 1, &queuePriority});
	}

	vk::DeviceCreateInfo createInfo =
	{
		{},
		(uint32_t)queueCreateInfos.size(),
		queueCreateInfos.data(),
#ifdef ENABLE_VALIDATION_LAYER
		(uint32_t)helper::ValidationLayer.size(),
		helper::ValidationLayer.data(),
#else
		0,
		nullptr,
#endif
		(uint32_t)helper::DeviceExtensions.size(),
		helper::DeviceExtensions.data(),
		{}
	};

	logicalDevice = physicalDevice.createDevice(createInfo, nullptr);

	graphicsQueue = logicalDevice.getQueue(indices.graphicsFamily, 0);
	presentQueue = logicalDevice.getQueue(indices.presentFamily, 0);
}

vk::Device lpe::SwapChain::GetLogicalDevice() const
{
	return logicalDevice;
}

vk::PhysicalDevice lpe::SwapChain::GetPhysicalDevice() const
{
	return physicalDevice;
}

vk::Format lpe::SwapChain::GetSwapChainImageFormat() const
{
	return imageFormat;
}

vk::Extent2D lpe::SwapChain::GetSwapChainExtent() const
{
	return extent;
}

vk::Queue lpe::SwapChain::GetGraphicsQueue() const
{
	return graphicsQueue;
}

vk::Queue lpe::SwapChain::GetPresentQueue() const
{
	return presentQueue;
}

vk::SurfaceFormatKHR lpe::SwapChain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const
{
	if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
	{
		return{ vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto& availableFormat : formats)
	{
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}

	return formats[0];
}

vk::PresentModeKHR lpe::SwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes) const
{
	for (const auto& presentMode : presentModes)
	{
		if (presentMode == vk::PresentModeKHR::eMailbox)
		{
			return presentMode;
		}
	}

	return vk::PresentModeKHR::eImmediate;
}

vk::Extent2D lpe::SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const uint32_t width, const uint32_t height) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		vk::Extent2D actualExtent = { width, height };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;

	}
}

void lpe::SwapChain::CreateSwapChain(const uint32_t physicalDeviceIndex,
                                     const vk::Instance& instance,
                                     const uint32_t width,
                                     const uint32_t height)
{
	this->instance = instance;

	PickPhysicalDevice(physicalDeviceIndex);

	CreateLogicalDevice();

	SwapChainSupportDetails details = QuerySwapChainDetails(physicalDevice);

	vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
	vk::PresentModeKHR presentMode = ChooseSwapPresentMode(details.presentModes);
	vk::Extent2D extent = ChooseSwapExtent(details.capabilities, width, height);

	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
	{
		imageCount = details.capabilities.maxImageCount;
	}

	vk::SwapchainKHR oldSwapChain = swapchain;
	vk::SwapchainCreateInfoKHR createInfo =
	{
		{},
		surface,
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive,
		0,
		nullptr,
		details.capabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		presentMode,
		VK_TRUE,
		oldSwapChain
	};

	auto indices = FindQueueFamilies(physicalDevice);
	auto queueFamilyIndices = indices.GetAsArray();

	if(indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = (uint32_t)queueFamilyIndices.size();
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	}

	vk::SwapchainKHR newSwapchain = logicalDevice.createSwapchainKHR(createInfo, nullptr);
	swapchain = newSwapchain;

	imageFormat = surfaceFormat.format;
	this->extent = extent;
}

void lpe::SwapChain::CreateImageViews()
{
	auto swapchainImages = logicalDevice.getSwapchainImagesKHR(swapchain);

	imageViews.resize(swapchainImages.size(), ImageView {physicalDevice, logicalDevice});

	for (int i = 0; i < swapchainImages.size(); i++)
	{
		imageViews[i].Create(swapchainImages[i], imageFormat, vk::ImageAspectFlagBits::eColor);
	}
}

void lpe::SwapChain::Init(std::string appName, 
						  GLFWwindow* window,
						  const uint32_t width,
						  const uint32_t height)
{
	this->Init(appName, window, width, height, -1);
}

void lpe::SwapChain::Init(std::string appName, 
						  GLFWwindow* window, 
						  const uint32_t width, 
						  const uint32_t height, 
						  const uint32_t physicalDeviceIndex)
{
	vk::Instance instance = lpe::CreateInstance(appName, nullptr, &callback);

	if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}

	CreateSwapChain(physicalDeviceIndex, instance, width, height);

	CreateImageViews();
}

void lpe::SwapChain::CreateFrameBuffers(const lpe::ImageView& depthImage, const vk::RenderPass& renderPass)
{
	framebuffers.resize(imageViews.size());

	for(size_t i = 0; i < imageViews.size(); i++)
	{
		std::array<vk::ImageView, 2> attachments = {imageViews[i].GetImageView(), depthImage.GetImageView()};

		vk::FramebufferCreateInfo framebufferInfo = { {}, renderPass, (uint32_t)attachments.size(), attachments.data(), extent.width, extent.height, 1 };

		auto result = logicalDevice.createFramebuffer(&framebufferInfo, nullptr, &framebuffers[i]);

		if (result != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to create framebuffer! (" + vk::to_string(result) + ")");
		}
	}
}

QueueFamilyIndices lpe::SwapChain::FindQueueFamilies() const
{
	return FindQueueFamilies(physicalDevice);
}
