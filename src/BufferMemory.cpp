#include "../include/BufferMemory.h"

lpe::BufferMemory::BufferMemory(const BufferMemory& other)
{
  device.reset(other.device.get());
  physicalDevice = other.physicalDevice;
  buffers = { other.buffers };
  memory = other.memory;
  size = other.size;
  alignments = { other.alignments };
  mapped = other.mapped;
  usageFlags = { other.usageFlags };
  memoryPropertyFlags = other.memoryPropertyFlags;
  offsets = { other.offsets };
}

lpe::BufferMemory::BufferMemory(BufferMemory&& other) noexcept
{
  device = std::move(other.device);
  physicalDevice = other.physicalDevice;
  buffers = std::move(other.buffers);
  memory = other.memory;
  size = other.size;
  alignments = std::move(other.alignments);
  mapped = other.mapped;
  usageFlags = std::move(other.usageFlags);
  memoryPropertyFlags = other.memoryPropertyFlags;
  offsets = std::move(other.offsets);
}

lpe::BufferMemory& lpe::BufferMemory::operator=(const BufferMemory& other)
{
  device.reset(other.device.get());
  physicalDevice = other.physicalDevice;
  buffers = { other.buffers };
  memory = other.memory;
  size = other.size;
  alignments = { other.alignments };
  mapped = other.mapped;
  usageFlags = { other.usageFlags };
  memoryPropertyFlags = other.memoryPropertyFlags;
  offsets = { other.offsets };

  return *this;
}

lpe::BufferMemory& lpe::BufferMemory::operator=(BufferMemory&& other) noexcept
{
  device = std::move(other.device);
  physicalDevice = other.physicalDevice;
  buffers = std::move(other.buffers);
  memory = other.memory;
  size = other.size;
  alignments = std::move(other.alignments);
  mapped = other.mapped;
  usageFlags = std::move(other.usageFlags);
  memoryPropertyFlags = other.memoryPropertyFlags;
  offsets = std::move(other.offsets);

  return *this;
}

lpe::BufferMemory::~BufferMemory()
{
  Destroy();
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
  mapped = nullptr;
}

void lpe::BufferMemory::Bind(uint32_t id,
                             vk::DeviceSize offset)
{
  device->bindBufferMemory(buffers.at(id),
                           memory,
                           offset);
}

vk::DescriptorBufferInfo lpe::BufferMemory::SetupDescriptor(uint32_t id,
                                                            vk::DeviceSize size,
                                                            vk::DeviceSize offset)
{
  descriptor.offset = offset;
  descriptor.range = size;
  descriptor.buffer = buffers.at(id);

  return descriptor;
}

void lpe::BufferMemory::Write(void* data,
                              vk::DeviceSize size,
                              vk::DeviceSize offset,
                              bool map)
{
  if (map)
  {
    auto result = Map(size, offset);
    helper::ThrowIfNotSuccess(result,
                              "Can't map memory");
  }

  assert(mapped);

  memcpy(mapped,
         data,
         size);

  if (map && mapped)
  {
    Unmap();
  }
}

void lpe::BufferMemory::WriteStaged(uint32_t sourceId,
                                    uint32_t targetId,
                                    void* data,
                                    vk::DeviceSize size,
                                    BufferMemory& buffer,
                                    vk::DeviceSize offset,
                                    vk::CommandBuffer command)
{
  buffer.Write(data,
               size,
               offset);

  vk::BufferCopy copyRegion = { offset, 0, size };
  command.copyBuffer(buffer.GetBuffer(sourceId),
                     buffers[targetId],
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
    if (!buffers.empty())
    {
      for(auto iter = std::begin(buffers); iter != std::end(buffers); ++iter)
      {
        device->destroyBuffer(iter->second);
      }
    }

    if (memory)
    {
      device->freeMemory(memory);
    }
  }
}

vk::DeviceSize lpe::BufferMemory::GetSize(uint32_t id,
                                          uint32_t key)
{
  auto size = offsets[id][key];

  return size;
}

vk::DeviceSize lpe::BufferMemory::GetOffset(uint32_t id,
                                            uint32_t key)
{
  vk::DeviceSize offset = 0;
  auto alignment = alignments[id];

  for (const auto& off : offsets[id])
  {
    if (off.first == key)
    {
      break;
    }

    offset += off.second;
  }

  
  return offset;
}

void lpe::BufferMemory::Get(uint32_t id,
                            uint32_t key,
                            vk::DeviceSize* size,
                            vk::DeviceSize* offset)
{
  for (const auto& off : offsets[id])
  {
    if (off.first == key)
    {
      *size = off.second;
      break;
    }

    *offset += off.second;
  }
}

std::map<uint32_t, vk::DeviceSize> lpe::BufferMemory::GetOffsets(uint32_t id)
{
  return offsets[id];
}

vk::Buffer lpe::BufferMemory::GetBuffer(uint32_t id) const
{
  return buffers.at(id);
}

vk::DescriptorBufferInfo lpe::BufferMemory::GetDescriptor() const
{
  return descriptor;
}

vk::BufferUsageFlags lpe::BufferMemory::GetUsageFlags(uint32_t id) const
{
  auto found = usageFlags.find(id);
  if(found != std::end(usageFlags))
  {
    return (*found).second;
  }

  return {};
}

vk::MemoryPropertyFlags lpe::BufferMemory::GetMemoryPropertyFlags() const
{
  return memoryPropertyFlags;
}
