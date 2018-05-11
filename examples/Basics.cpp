#include <iostream>
#include "../src/lpe.h"

int main()
{
  lpe::window::Window window("Hello World",
                             1920,
                             1080,
                             true);

  window.KeyPressed += [](const lpe::window::Window& window,
                          const lpe::window::WindowKeyPressedEventArgs& e)
  {
    std::cout << static_cast<uint32_t>(e.GetKey()) << std::endl;
  };

  {
    lpe::vulkan::Instance instance("asdf", VK_MAKE_VERSION(0, 0, 1));
    auto surface = window.GetWindowSurface(instance);
    lpe::vulkan::PhysicalDevice physicalDevice = instance.GetSuitablePhysicalDevice(surface);
    lpe::vulkan::Device device = physicalDevice.CreateDevice();
    vk::Queue graphicsQueue = device.GetQueue(physicalDevice.GetQueueFamilyIndices().graphicsFamily.familyIndex);
    lpe::vulkan::SwapChain swapChain = device.CreateSwapChain();
  }

  while (window.IsOpen())
  {
    window.Render();
  }

  return 0;
}
