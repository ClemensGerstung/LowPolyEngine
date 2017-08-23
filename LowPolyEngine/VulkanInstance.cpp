#include "stdafx.h"
#include "VulkanInstance.h"
#include <GLFW/glfw3.h>
#include <iostream>

lpe::Vulkan::Vulkan(const std::string& applicationName, const uint32_t version, const std::shared_ptr<vk::AllocationCallbacks> allocator)
{
	this->instanceAllocator = allocator;

	if (EnableValidationLayer && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available");
	}

	auto extensions = this->GetRequiredExtensions();

	vk::ApplicationInfo info = { applicationName.c_str(), version, ENGINE_NAME, ENGINE_VERSION, VK_API_VERSION_1_0 };
	vk::InstanceCreateInfo createInfo = 
	{ 
		{}, 
		&info, 
		(uint32_t)(EnableValidationLayer ? ValidationLayer.size() : 0),
		(EnableValidationLayer ? ValidationLayer.data() : nullptr),
		(uint32_t)extensions.size(),
		extensions.data()
	};

	auto result = vk::createInstance(&createInfo, allocator.get(), &instance);
	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Couldn't create Vulkan instance (Error: " + vk::to_string(result) +")");
	}

	if(EnableValidationLayer)
	{
		this->callback = std::make_shared<vk::DebugReportCallbackEXT>();

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = debugCallback;

		if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, callback.get()))
		{
			throw std::runtime_error("Failed to set up debug callback");
		}
	}
}

lpe::Vulkan::~Vulkan()
{
	instance.destroySurfaceKHR(surface, nullptr);
	DestroyDebugReportCallbackEXT(instance, *callback.get(), nullptr);
	instance.destroy(instanceAllocator.get());
}

void lpe::Vulkan::CreateSurface(GLFWwindow* window)
{
	auto result = glfwCreateWindowSurface(static_cast<VkInstance>(instance), window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface));
	if(result != (VkResult)vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create window surface! (" + vk::to_string((vk::Result)result) +")");
	}
}

const lpe::Device& lpe::Vulkan::CreateDevice() const
{
	device = { &surface, instance.enumeratePhysicalDevices() };
	
	return device;
}


std::vector<const char*> lpe::Vulkan::GetRequiredExtensions()
{
	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++)
	{
		extensions.push_back(glfwExtensions[i]);
	}

	if (EnableValidationLayer)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

VkBool32 lpe::Vulkan::debugCallback(VkDebugReportFlagsEXT flags, 
									VkDebugReportObjectTypeEXT objType, 
									uint64_t obj, 
									size_t location, 
									int32_t code, 
									const char* layerPrefix, 
									const char* msg, 
									void* userData)
{
	std::cerr << "validaion layer: " << msg << std::endl;

	return VK_FALSE;
}
