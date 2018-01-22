#include "../include/BufferMemory.h"
#include <cstdarg>

lpe::BufferMemory::BufferMemory(const BufferMemory& other)
{
  device.reset(other.device.get());
  physicalDevice = other.physicalDevice;
  buffer = other.buffer;
  memory = other.memory;
  size = other.size;
  alignment = other.alignment;
  mapped = other.mapped;
  usageFlags = other.usageFlags;
  memoryPropertyFlags = other.memoryPropertyFlags;
  offsets = { other.offsets };
}

lpe::BufferMemory::BufferMemory(BufferMemory&& other) noexcept
{
  device = std::move(other.device);
  physicalDevice = other.physicalDevice;
  buffer = other.buffer;
  memory = other.memory;
  size = other.size;
  alignment = other.alignment;
  mapped = other.mapped;
  usageFlags = other.usageFlags;
  memoryPropertyFlags = other.memoryPropertyFlags;
  offsets = std::move(other.offsets);
}

lpe::BufferMemory& lpe::BufferMemory::operator=(const BufferMemory& other)
{
  device.reset(other.device.get());
  physicalDevice = other.physicalDevice;
  buffer = other.buffer;
  memory = other.memory;
  size = other.size;
  alignment = other.alignment;
  mapped = other.mapped;
  usageFlags = other.usageFlags;
  memoryPropertyFlags = other.memoryPropertyFlags;
  offsets = { other.offsets };

  return *this;
}

lpe::BufferMemory& lpe::BufferMemory::operator=(BufferMemory&& other) noexcept
{
  device = std::move(other.device);
  physicalDevice = other.physicalDevice;
  buffer = other.buffer;
  memory = other.memory;
  size = other.size;
  alignment = other.alignment;
  mapped = other.mapped;
  usageFlags = other.usageFlags;
  memoryPropertyFlags = other.memoryPropertyFlags;
  offsets = std::move(other.offsets);

  return *this;
}

lpe::BufferMemory::~BufferMemory()
{
  Destroy();
}

lpe::BufferMemory::BufferMemory(vk::Device* device,
                                vk::PhysicalDevice physicalDevice,
                                vk::BufferUsageFlags usage,
                                vk::MemoryPropertyFlags properties,
                                std::initializer_list<std::pair<uint32_t, vk::DeviceSize>> pairs)
  : physicalDevice(physicalDevice),
    usageFlags(usage),
    memoryPropertyFlags(properties)
{
  this->device.reset(device);
  size = 0;
  mapped = nullptr;

  for (auto pair : pairs)
  {
    offsets.insert(pair);
    size += pair.second;
  }

  vk::BufferCreateInfo createInfo = { {}, size, usage, vk::SharingMode::eExclusive };
  auto result = device->createBuffer(&createInfo,
                                     nullptr,
                                     &buffer);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create buffer!");

  vk::MemoryRequirements requirements = device->getBufferMemoryRequirements(buffer);
  auto memoryProperties = physicalDevice.getMemoryProperties();
  auto memoryIndex = helper::FindMemoryTypeIndex(requirements.memoryTypeBits,
                                                 properties,
                                                 memoryProperties);

  vk::MemoryAllocateInfo allocInfo = { requirements.size, memoryIndex };

  result = device->allocateMemory(&allocInfo,
                                  nullptr,
                                  &memory);
  helper::ThrowIfNotSuccess(result,
                            "Failed to allocate buffer memory!");
  alignment = requirements.alignment;

  SetupDescriptor();

  Bind();
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
  buffer.Write(data,
               size);

  vk::BufferCopy copyRegion = { offset, 0, size };
  command.copyBuffer(buffer.GetBuffer(),
                     this->buffer,
                     1,
                     &copyRegion);
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

vk::DeviceSize lpe::BufferMemory::GetSize(uint32_t key)
{
  return offsets[key];
}

vk::DeviceSize lpe::BufferMemory::GetOffset(uint32_t key)
{
  vk::DeviceSize offset = 0;

  for (const auto& off : offsets)
  {
    if(off.first == key)
    {
      break;
    }

    offset += off.second;
  }

  return offset;
}

void lpe::BufferMemory::Get(uint32_t key,
                            vk::DeviceSize* size,
                            vk::DeviceSize* offset)
{
  for (const auto& off : offsets)
  {
    if (off.first == key)
    {
      *size = off.second;
      break;
    }

    *offset += off.second;
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
