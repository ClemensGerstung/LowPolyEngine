#include "SwapChain.h"
#include "lpe.h"


void lpe::SwapChain::CreateSwapChain(const uint32_t physicalDeviceIndex,
                                     const vk::Instance& instance,
                                     const uint32_t width,
                                     const uint32_t height)
{

}

lpe::SwapChain::SwapChain(std::string appName,
                          const uint32_t width,
                          const uint32_t height)
{
	vk::Instance instance = lpe::CreateInstance(appName);
	CreateSwapChain(0, instance, width, height);
}

lpe::SwapChain::SwapChain(const uint32_t physicalDeviceIndex,
                          const vk::Instance& instance,
                          const uint32_t width,
                          const uint32_t height)
{
	CreateSwapChain(physicalDeviceIndex, instance, width, height);
}
