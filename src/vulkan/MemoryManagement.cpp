#include "MemoryManagement.hpp"
#include "VulkanManager.hpp"
#include "../ServiceLocator.h"

vk::Buffer lpe::rendering::vulkan::common::CreateBuffer(vk::Device device, vk::DeviceSize size, vk::BufferUsageFlags usage, const uint32_t *queueFamilies, uint32_t queueFamilyCount)
{
  vk::Buffer buffer = nullptr;
  vk::BufferCreateInfo createInfo =
    {
      {},
      size,
      usage,
      queueFamilyCount > 0 ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
      queueFamilyCount,
      queueFamilies
    };

  auto result = device.createBuffer(&createInfo, nullptr, &buffer);
  if (result != vk::Result::eSuccess)
  {
    auto mgr = lpe::ServiceLocator::LogManager.Get()
                                              .lock();

    if (mgr)
    {
      mgr->Log("Could not create Buffer");
    }

    buffer = nullptr;
  }

  return buffer;
}

uint32_t lpe::rendering::vulkan::common::GetMemoryType(vk::PhysicalDeviceMemoryProperties deviceProperties,
                                                       vk::MemoryRequirements requirements,
                                                       vk::MemoryPropertyFlags properties,
                                                       uint32_t typeOffset)
{
  uint32_t type = std::numeric_limits<uint32_t>::max(); // 0xFFFFFFFF

  for (uint32_t index = typeOffset; index < deviceProperties.memoryTypeCount; ++index)
  {
    if ((requirements.memoryTypeBits & (1 << index)) &&
        (deviceProperties.memoryTypes[index].propertyFlags & properties) == properties)
    {
      type = index;
      break;
    }
  }

  return type;
}

uint32_t lpe::rendering::vulkan::common::AllocateMemory(vk::Device device,
                                                        vk::PhysicalDevice physicalDevice,
                                                        vk::MemoryRequirements requirements,
                                                        vk::MemoryPropertyFlags properties,
                                                        vk::DeviceMemory *memory)
{
  const vk::PhysicalDeviceMemoryProperties& deviceProperties = physicalDevice.getMemoryProperties();
  uint32_t type = 0;
  vk::Result result = vk::Result::eSuccess;

  do
  {
    type = GetMemoryType(deviceProperties,
                         requirements,
                         properties,
                         type);

    vk::MemoryAllocateInfo allocateInfo = {
      requirements.size,
      type
    };

    result = device.allocateMemory(&allocateInfo,
                                   nullptr,
                                   memory);
  } while (result != vk::Result::eSuccess &&
           type < deviceProperties.memoryTypeCount);

  if (!(*memory))
  {
    auto mgr = lpe::ServiceLocator::LogManager.Get()
                                              .lock();
    if (mgr)
    {
      mgr->Log("Could not allocate memory");
    }
  }

  return type;
}

bool lpe::rendering::vulkan::common::CreateBufferAllocateAndBindMemory(vk::Device device,
                                                                       vk::PhysicalDevice physicalDevice,
                                                                       vk::Buffer& buffer,
                                                                       vk::DeviceMemory& memory,
                                                                       uint32_t& memoryType,
                                                                       vk::DeviceSize size,
                                                                       vk::BufferUsageFlags usage,
                                                                       vk::MemoryPropertyFlags properties,
                                                                       const uint32_t *queueFamilies,
                                                                       uint32_t queueFamilyCount)
{
  auto mgr = lpe::ServiceLocator::LogManager.Get()
                                            .lock();

  buffer = CreateBuffer(device, size, usage, queueFamilies, queueFamilyCount);
  if (!buffer)
  {
    if (mgr)
    {
      mgr->Log("Could create Buffer (combined)");
    }

    return false;
  }

  auto requirements = device.getBufferMemoryRequirements(buffer);

  memoryType = AllocateMemory(device,
                              physicalDevice,
                              requirements,
                              properties,
                              &memory);

  if (!memory)
  {
    if (mgr)
    {
      mgr->Log("Could allocate memory (combined)");
    }

    return false;
  }

  device.bindBufferMemory(buffer, memory, 0);

  return true;
}

void lpe::rendering::vulkan::StackAllocator::Create(std::shared_ptr<lpe::rendering::vulkan::VulkanManager> manager,
                                                    vk::DeviceSize size,
                                                    vk::MemoryPropertyFlags properties,
                                                    vk::BufferUsageFlags usage)
{
  this->logger = lpe::ServiceLocator::LogManager.Get();
  this->manager = manager;
  this->device = manager->GetDevice();
  this->size = size;
  this->properties = properties;
  this->memory = nullptr;
  this->offset = 0;
  this->marker = 0;
  auto physicalDevice = manager->GetPhysicalDevice();

  bool result = common::CreateBufferAllocateAndBindMemory(this->device,
                                                          physicalDevice,
                                                          this->buffer,
                                                          this->memory,
                                                          this->memoryType,
                                                          this->size,
                                                          usage,
                                                          properties);

  if (!result)
  {
    {
      auto ptr = logger.lock();

      if (ptr)
      {
        ptr->Log("Could not initialize StackAllocator");
      }
    }

    assert(buffer);
    assert(memory);
  }
}

void lpe::rendering::vulkan::StackAllocator::Destroy()
{
  if(memory)
  {
    device.free(memory);
    memory = nullptr;
  }

  if(buffer)
  {
    device.destroyBuffer(buffer);
    buffer = nullptr;
  }

  offset = 0;
  marker = 0;
  size = 0;

  logger.reset();
  manager.reset();
}

vk::DeviceSize lpe::rendering::vulkan::StackAllocator::Push(void *data, vk::DeviceSize size, lpe::rendering::vulkan::MarkerPosition pos)
{
  vk::DeviceSize begin = offset;

  assert((begin + size) <= this->size);

  offset = offset + size;

  void* memory;
  auto result = device.mapMemory(this->memory, begin, size, vk::MemoryMapFlags(), &memory);

  assert(result == vk::Result::eSuccess);

  memcpy(memory, data, size);

  device.unmapMemory(this->memory);

  if (pos == MarkerPosition::Before)
  {
    marker = begin;
  }
  else if (pos == MarkerPosition::After)
  {
    marker = offset;
  }

  return begin;
}

vk::DeviceSize lpe::rendering::vulkan::StackAllocator::Pop(bool complete)
{
  offset = complete ?
           0 :
           marker;

  return offset;
}

void lpe::rendering::vulkan::StackAllocator::SetMarker(vk::DeviceSize offset)
{
  this->marker = offset;
}

void lpe::rendering::vulkan::StackAllocator::RemoveMarker()
{
  this->marker = 0;
}

bool lpe::rendering::vulkan::StackAllocator::Fits(vk::DeviceSize size) const
{
  return offset + size <= this->size;
}

lpe::rendering::vulkan::StackAllocator::StackAllocator()
{
  this->device = nullptr;
  this->buffer = nullptr;
  this->memory = nullptr;
  this->size = 0;
  this->offset = 0;
  this->marker = 0;
  this->properties = {};
  this->memoryType = 0;
}


