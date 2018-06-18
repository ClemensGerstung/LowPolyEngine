#pragma once

#include "DeviceLevelObject.h"

namespace lpe
{
  namespace vulkan
  {
    class SwapChain : protected DeviceLevelObject
    {
    private:
      vk::SurfaceKHR surface;
      vk::PresentModeKHR presentMode;
      vk::SwapchainKHR swapchain;
      vk::Extent2D extent;
      vk::Format format;
      vk::ColorSpaceKHR colorSpace;
      std::vector<vk::Image> swapChainImages;
    public:
      SwapChain() = default;
      SwapChain(const SwapChain& other);
      SwapChain(SwapChain&& other) noexcept;
      SwapChain& operator=(const SwapChain& other);
      SwapChain& operator=(SwapChain&& other) noexcept;
      SwapChain(Instance* instance,
                PhysicalDevice* physicalDevice,
                Device* device,
                vk::SurfaceKHR surface,
                vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo,
                vk::Format format = vk::Format::eB8G8R8A8Unorm,
                vk::ColorSpaceKHR colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear);
      ~SwapChain();

      vk::Image AquireNextImage(vk::Semaphore semaphore, vk::Fence fence, uint32_t *index);
      void Present(uint32_t index, uint32_t presentQueueFamilyIndex, const std::vector<vk::Semaphore>& semaphores);
      void Recreate();
    };
  }
}
