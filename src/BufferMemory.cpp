#include "../include/BufferMemory.h"
#include <cstdarg>

lpe::BufferMemory::BufferMemory(const BufferMemory& other)
{
}

lpe::BufferMemory::BufferMemory(BufferMemory&& other) noexcept
{
}

lpe::BufferMemory& lpe::BufferMemory::operator=(const BufferMemory& other)
{
  return *this;
}

lpe::BufferMemory& lpe::BufferMemory::operator=(BufferMemory&& other) noexcept
{
  return *this;
}

lpe::BufferMemory::~BufferMemory()
{
  Destroy();
}

lpe::BufferMemory::BufferMemory(vk::Device* device,
                                vk::PhysicalDevice physicalDevice,
                                std::initializer_list<std::pair<uint32_t, vk::DeviceSize>> pairs)
{
  this->device.reset(device);
  this->physicalDevice = physicalDevice;
  size = 0;
  mapped = nullptr;

  for (auto pair : pairs)
  {
    offsets.insert(pair);
    size += pair.second;
  }
}


vk::Result lpe::BufferMemory::Map(vk::DeviceSize size,
                                  vk::DeviceSize offset)
{
  return device->mapMemory(memory,
                           offset,
                           size,
                           {},
                           &mapped);
}

void lpe::BufferMemory::Unmap()
{
  device->unmapMemory(memory);
}

void lpe::BufferMemory::Bind(vk::DeviceSize offset)
{
  device->bindBufferMemory(buffer,
                           memory,
                           offset);
}

vk::DescriptorBufferInfo lpe::BufferMemory::SetupDescriptor(vk::DeviceSize size,
                                                            vk::DeviceSize offset)
{
  descriptor.offset = offset;
  descriptor.range = size;
  descriptor.buffer = buffer;

  return descriptor;
}

void lpe::BufferMemory::Write(void* data,
                              vk::DeviceSize size,
                              bool map)
{
  assert(mapped);

  if (map)
  {
    auto result = Map();
  }

  memcpy(mapped,
         data,
         size);

  if (map)
  {
    Unmap();
  }
}

void lpe::BufferMemory::WriteStaged(void* data,
                                    vk::DeviceSize size,
                                    BufferMemory& buffer,
                                    vk::DeviceSize offset,
                                    vk::CommandBuffer command)
{
  buffer.Write(data, size);

  vk::BufferCopy copyRegion = {offset, 0, size};
  command.copyBuffer(buffer.GetBuffer(), this->buffer, 1, &copyRegion);
}


vk::Result lpe::BufferMemory::Flush(vk::DeviceSize size,
                                    vk::DeviceSize offset)
{
  vk::MappedMemoryRange mappedRange = { memory, offset, size };

  return device->flushMappedMemoryRanges(1,
                                         &mappedRange);
}

vk::Result lpe::BufferMemory::Invalidate(vk::DeviceSize size,
                                         vk::DeviceSize offset)
{
  vk::MappedMemoryRange mappedRange = { memory, offset, size };

  return device->invalidateMappedMemoryRanges(1,
                                              &mappedRange);
}

void lpe::BufferMemory::Destroy()
{
  if (device)
  {
    if (buffer)
    {
      device->destroyBuffer(buffer);
    }

    if (memory)
    {
      device->freeMemory(memory);
    }
  }
}

vk::Buffer lpe::BufferMemory::GetBuffer() const
{
  return buffer;
}

vk::DescriptorBufferInfo lpe::BufferMemory::GetDescriptor() const
{
  return descriptor;
}

vk::BufferUsageFlags lpe::BufferMemory::GetUsageFlags() const
{
  return usageFlags;
}

vk::MemoryPropertyFlags lpe::BufferMemory::GetMemoryPropertyFlags() const
{
  return memoryPropertyFlags;
}
