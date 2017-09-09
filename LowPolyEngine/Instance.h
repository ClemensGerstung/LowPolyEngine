#ifndef INSTANCE_H
#define INSTANCE_H

#include "stdafx.h"
#include "lpe.h"
#include "Device.h"
#include "SwapChainSupportDetails.h"

BEGIN_LPE

class Instance
{
private:
	vk::Instance instance;
	std::vector<vk::PhysicalDevice> physicalDevices;
	vk::DebugReportCallbackEXT callback;

  bool IsDeviceSuitable(vk::PhysicalDevice device, const vk::SurfaceKHR& surface) const;
  vk::PhysicalDevice PickPhysicalDevice(const uint32_t physicalDeviceIndex, const vk::SurfaceKHR& surface);

public:
	Instance() = default;
	Instance(const Instance&) = delete;
	Instance(Instance&&) = delete;
	Instance operator =(const Instance&) = delete;
	Instance operator =(Instance&&) = delete;
	~Instance();

	void Create(const std::string& appName);

	lpe::Device CreateDevice(GLFWwindow* window, const uint32_t physicalDeviceIndex = -1);

  // TODO: move them into lpe::helpers in stdafx.h ?
  static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, const vk::SurfaceKHR& surface);
  static bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
  static SwapChainSupportDetails QuerySwapChainDetails(vk::PhysicalDevice device, const vk::SurfaceKHR& surface);
};

END_LPE

#endif