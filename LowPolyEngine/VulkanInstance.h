#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "VDeleter.h"
#include "VulkanDevice.h"

namespace lpe {
	class Vulkan
	{
	private:
		vk::Instance instance;
		std::shared_ptr<vk::AllocationCallbacks> instanceAllocator;
		std::shared_ptr<vk::DebugReportCallbackEXT> callback;
		vk::SurfaceKHR surface;
		Device device;

	public:
		Vulkan() = delete;
		Vulkan(const Vulkan&) = delete;
		Vulkan(Vulkan&&) = delete;
		Vulkan operator=(const Vulkan&) = delete;
		Vulkan operator=(Vulkan&&) = delete;

		Vulkan(const std::string& applicationName, 
			   const uint32_t version, 
			   const std::shared_ptr<vk::AllocationCallbacks> allocator = nullptr);
		~Vulkan();

		void CreateSurface(GLFWwindow* window);
		const Device& CreateDevice() const;

		static std::vector<const char*> GetRequiredExtensions();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, 
															VkDebugReportObjectTypeEXT objType, 
															uint64_t obj, 
															size_t location, 
															int32_t code, 
															const char* layerPrefix, 
															const char* msg, 
															void* userData);
	};

}

#endif