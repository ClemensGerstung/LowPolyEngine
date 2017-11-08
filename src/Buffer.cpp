#include "../include/Buffer.h"
#include "../include/Commands.h"

void lpe::Buffer::CreateBuffer(vk::DeviceSize size,
                               vk::BufferUsageFlags usage,
                               vk::MemoryPropertyFlags properties)
{
  vk::BufferCreateInfo createInfo = { {}, size, usage, vk::SharingMode::eExclusive };

  auto result = device->createBuffer(&createInfo, nullptr, &buffer);
  if (result != vk::Result::eSuccess)
  {
    throw std::runtime_error("failed to create buffer!" + vk::to_string(result) + "");
  }

  vk::MemoryRequirements requirements = device->getBufferMemoryRequirements(buffer);

  vk::MemoryAllocateInfo allocInfo = { requirements.size, helper::FindMemoryTypeIndex(requirements.memoryTypeBits, properties, physicalDevice.getMemoryProperties()) };

  result = device->allocateMemory(&allocInfo, nullptr, &memory);
  
  if (result != vk::Result::eSuccess)
  {
    throw std::runtime_error("failed to allocate buffer memory!" + vk::to_string(result) + "");
  }

  device->bindBufferMemory(buffer, memory, 0);
}

lpe::Buffer::Buffer(const Buffer& other)
{
  this->device.reset(other.device.get());
  this->physicalDevice = other.physicalDevice;
  this->size = other.size;
  this->buffer = other.buffer;
  this->memory = other.memory;
  this->descriptor = other.descriptor;
  this->mapped = other.mapped;
}

lpe::Buffer::Buffer(Buffer&& other)
{
  this->device = std::move(other.device);

  this->physicalDevice = other.physicalDevice;
  this->size = other.size;
  this->buffer = other.buffer;
  this->memory = other.memory;
  this->descriptor = other.descriptor;
  this->mapped = other.mapped;
}

lpe::Buffer& lpe::Buffer::operator=(const Buffer& other)
{
  this->device.reset(other.device.get());
  this->physicalDevice = other.physicalDevice;
  this->size = other.size;
  this->buffer = other.buffer;
  this->memory = other.memory;
  this->descriptor = other.descriptor;
  this->mapped = other.mapped;

  return *this;
}

lpe::Buffer& lpe::Buffer::operator=(Buffer&& other)
{
  this->device.swap(other.device);
  other.device.release();

  this->physicalDevice = other.physicalDevice;
  this->size = other.size;
  this->buffer = other.buffer;
  this->memory = other.memory;
  this->descriptor = other.descriptor;
  this->mapped = other.mapped;

  return *this;
}

lpe::Buffer::Buffer(vk::PhysicalDevice physicalDevice, vk::Device* device)
  : physicalDevice(physicalDevice),
    size(VK_WHOLE_SIZE),
    mapped(nullptr)
{
  this->device.reset(device);

  descriptor = vk::DescriptorBufferInfo{ buffer, 0, VK_WHOLE_SIZE };
}

lpe::Buffer::Buffer(vk::PhysicalDevice physicalDevice,
                    vk::Device* device,
                    void* data,
                    vk::DeviceSize size,
                    vk::BufferUsageFlags usage,
                    vk::MemoryPropertyFlags properties)
  : physicalDevice(physicalDevice),
    size(size)
{
  this->device.reset(device);

  CreateBuffer(size, usage, properties);

  this->device->mapMemory(memory, 0, size, {}, &mapped);
  memcpy(mapped, data, (size_t)size);
  this->device->unmapMemory(memory);

  descriptor = vk::DescriptorBufferInfo{buffer, 0, VK_WHOLE_SIZE};
}

lpe::Buffer::Buffer(vk::PhysicalDevice physicalDevice,
                    vk::Device* device,
                    vk::DeviceSize size,
                    vk::BufferUsageFlags usage,
                    vk::MemoryPropertyFlags properties)
  : physicalDevice(physicalDevice),
    size(size)
{
  this->device.reset(device);

  CreateBuffer(size, usage, properties);

  descriptor = vk::DescriptorBufferInfo{buffer, 0, VK_WHOLE_SIZE};
}

lpe::Buffer::~Buffer()
{
  if(device)
  {
    if(buffer)
    {
      device->destroyBuffer(buffer);
    }

    if(memory)
    {
      device->freeMemory(memory);
    }

    device.release();
  }
}

void lpe::Buffer::CreateHostVisible(vk::DeviceSize size, void* data, vk::BufferUsageFlags usage)
{
  this->size = size;

  CreateBuffer(size, usage, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  this->device->mapMemory(memory, 0, size, {}, &mapped);
  memcpy(mapped, data, (size_t)size);
  this->device->unmapMemory(memory);
}

void lpe::Buffer::CreateStaged(const Commands& commands, vk::DeviceSize size, void* data, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
{
  Buffer staging = { physicalDevice, device.get(), data, size };

  this->size = size;
  CreateBuffer(size, usage, properties);

  auto commandBuffer = commands.BeginSingleTimeCommands();

  Copy(staging, commandBuffer);

  commands.EndSingleTimeCommands(commandBuffer);
}

void lpe::Buffer::Copy(lpe::Buffer& src, vk::CommandBuffer& commandBuffer) const
{
  if(src.size != size)
  {
    throw std::runtime_error("copying a buffer to another buffer with another size is not possible");
  }

  vk::BufferCopy copyRegion = { 0, 0, size };

  commandBuffer.copyBuffer(src.buffer, buffer, 1, &copyRegion);
}

void lpe::Buffer::CopyStaged(vk::CommandBuffer& commandBuffer, void* data)
{
  Buffer staging = { physicalDevice, device.get(), data, size };

  Copy(staging, commandBuffer);
}

void lpe::Buffer::CopyToBufferMemory(void* data, size_t size)
{
  device->mapMemory(memory, 0, size, {}, &mapped);
  memcpy(mapped, data, size);
  device->unmapMemory(memory);
}

void lpe::Buffer::CopyToBufferMemory(void* data)
{
  CopyToBufferMemory(data, size);
}

vk::Buffer lpe::Buffer::GetBuffer()
{
  return buffer;
}

vk::DeviceMemory* lpe::Buffer::GetMemory()
{
  return &memory;
}

vk::DescriptorBufferInfo lpe::Buffer::GetDescriptor()
{
  return descriptor;
}

vk::DeviceSize lpe::Buffer::GetSize()
{
  return size;
}
