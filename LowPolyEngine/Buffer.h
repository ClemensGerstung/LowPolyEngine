#ifndef BUFFER_H
#define BUFFER_H
#include "stdafx.h"

BEGIN_LPE

class Commands;

class Buffer
{
private:
  vk::PhysicalDevice physicalDevice;
  std::unique_ptr<vk::Device> device;

  vk::Buffer buffer;
  vk::DeviceMemory memory;
  vk::DeviceSize size;
  vk::DescriptorBufferInfo descriptor;
  void* mapped;

protected:
  void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

public:
  Buffer() = default;
  Buffer(const Buffer& other);
  Buffer(Buffer&& other);
  Buffer& operator=(const Buffer& other);
  Buffer& operator=(Buffer&& other);

  Buffer(vk::PhysicalDevice physicalDevice, vk::Device* device);
  Buffer(vk::PhysicalDevice physicalDevice,
         vk::Device* device,
         void* data,
         vk::DeviceSize size,
         vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferSrc,
         vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
  Buffer(vk::PhysicalDevice physicalDevice,
         vk::Device* device,
         vk::DeviceSize size,
         vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eUniformBuffer,
         vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  ~Buffer();

  void Create(const Commands& commands, vk::DeviceSize size, void* data, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

  void Copy(lpe::Buffer& src, vk::CommandBuffer& commandBuffer) const;

  void CopyToBufferMemory(void* data, size_t size);
  void CopyToBufferMemory(void* data);

  vk::Buffer* GetBuffer();
  vk::DeviceMemory* GetMemory();
  vk::DescriptorBufferInfo* GetDescriptor();
  vk::DeviceSize GetSize();
};

END_LPE

#endif
