#include "lpe.h"

vk::Instance lpe::vulkan::DeviceLevelObject::GetInstance() const
{
  return *instance;
}

vk::PhysicalDevice lpe::vulkan::DeviceLevelObject::GetPhysicalDevice() const
{
  return *physicalDevice;
}

vk::Device lpe::vulkan::DeviceLevelObject::GetDevice() const
{
  return *device;
}

lpe::vulkan::DeviceLevelObject::DeviceLevelObject(const DeviceLevelObject& other)
{
  instance.reset(other.instance.get());
  physicalDevice.reset(other.physicalDevice.get());
  device.reset(other.device.get());
}

lpe::vulkan::DeviceLevelObject::DeviceLevelObject(DeviceLevelObject&& other) noexcept
{
  instance = std::move(other.instance);
  physicalDevice = std::move(other.physicalDevice);
  device = std::move(other.device);
}

lpe::vulkan::DeviceLevelObject& lpe::vulkan::DeviceLevelObject::operator=(const DeviceLevelObject& other)
{
  instance.reset(other.instance.get());
  physicalDevice.reset(other.physicalDevice.get());
  device.reset(other.device.get());
  return *this;
}

lpe::vulkan::DeviceLevelObject& lpe::vulkan::DeviceLevelObject::operator=(DeviceLevelObject&& other) noexcept
{
  instance = std::move(other.instance);
  physicalDevice = std::move(other.physicalDevice);
  device = std::move(other.device);
  return *this;
}

lpe::vulkan::DeviceLevelObject::DeviceLevelObject(Instance* instance,
                                                  PhysicalDevice* physicalDevice,
                                                  Device* device)
{
  this->instance.reset(instance);
  this->physicalDevice.reset(physicalDevice);
  this->device.reset(device);
}
