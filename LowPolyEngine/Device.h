#ifndef DEVICE_H
#define DEVICE_H

#include "stdafx.h"
#include "QueueFamilyIndices.h"
#include "SwapChain.h"
#include "Commands.h"

BEGIN_LPE

class Device
{
private:
  std::unique_ptr<vk::Instance> instance;
  vk::PhysicalDevice physicalDevice;
  vk::Device device;
  vk::SurfaceKHR surface;
  vk::Queue presentQueue;
  vk::Queue graphicsQueue;

  QueueFamilyIndices indices;

public:
  Device() = default;
  Device(const Device&);
  Device(Device&&);
  Device & operator =(const Device&);
  Device & operator =(Device&&);

  Device(vk::Instance* instance, vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR& surface);
  ~Device();

  SwapChain CreateSwapChain(uint32_t width, uint32_t height);
  Commands CreateCommands();

  explicit operator bool() const;
  bool operator!() const;
};

END_LPE

#endif
