#ifndef BUFFER_H
#define BUFFER_H
#include "stdafx.h"

BEGIN_LPE

class Buffer
{
private:
  vk::PhysicalDevice physicalDevice;
  std::unique_ptr<vk::Device> device;

  vk::Buffer buffer;
  vk::DeviceMemory memory;
  vk::DeviceSize size;

protected:
  void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

public:
  Buffer() = default;
  Buffer(const Buffer& other);
  Buffer(Buffer&& other);
  Buffer& operator=(const Buffer& other);
  Buffer& operator=(Buffer&& other);

  Buffer(vk::PhysicalDevice physicalDevice,
         vk::Device* device,
         void* data,
         vk::DeviceSize size,
         vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferSrc,
         vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
  Buffer(vk::PhysicalDevice physicalDevice,
         vk::Device* device,
         vk::DeviceSize size,
         vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferSrc,
         vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  ~Buffer();

  void Copy(const Buffer& src, vk::CommandBuffer commandBuffer);

  std::unique_ptr<vk::Buffer> GetBuffer();
};

END_LPE

#endif
