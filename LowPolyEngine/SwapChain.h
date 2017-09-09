#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "stdafx.h"
#include "QueueFamilyIndices.h"
#include "ImageView.h"

BEGIN_LPE

class SwapChain
{
private:
  vk::PhysicalDevice physicalDevice;
  std::unique_ptr<vk::Device> device;

  vk::SwapchainKHR swapchain;
  vk::Extent2D extent;
  vk::Format imageFormat;
  
  std::vector<lpe::ImageView> imageViews;

  vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const;
  vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes) const;
  vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const uint32_t width, const uint32_t height) const;

  void CreateSwapChain(vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR& surface, lpe::QueueFamilyIndices indices, uint32_t width, uint32_t height);
  void CreateImageViews();

public:
  SwapChain() = default;
  SwapChain(const SwapChain& other);
  SwapChain(SwapChain&& other);
  SwapChain& operator=(const SwapChain& other);
  SwapChain& operator=(SwapChain&& other);

  SwapChain(vk::PhysicalDevice physicalDevice, vk::Device* device, const vk::SurfaceKHR& surface, QueueFamilyIndices indices, uint32_t width, uint32_t height);

  ~SwapChain();
};

END_LPE

#endif
