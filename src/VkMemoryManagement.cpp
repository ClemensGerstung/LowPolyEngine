#include "VkMemoryManagement.h"

lpe::render::Chunk::~Chunk()
{
  assert(!memory);
}

const vk::DeviceMemory& lpe::render::Chunk::Create(vk::Device device,
                                                   vk::PhysicalDevice physicalDevice,
                                                   vk::MemoryRequirements requirements,
                                                   vk::MemoryPropertyFlagBits properties)
{
  this->device = device;
  this->memory = nullptr;
  this->size = requirements.size;
  this->alignment = requirements.alignment;
  this->properties = properties;

  assert(this->device);
  assert(this->size > 0);

  auto deviceMemoryProperties = physicalDevice.getMemoryProperties();

  for (uint32_t type = 0; type < deviceMemoryProperties.memoryTypeCount; ++type)
  {
    if ((requirements.memoryTypeBits & (1 << type)) &&
      ((deviceMemoryProperties.memoryTypes[type].propertyFlags & properties) == properties))
    {
      vk::MemoryAllocateInfo allocInfo =
      {
        this->size,
        type
      };

      auto result = device.allocateMemory(&allocInfo,
                                          nullptr,
                                          &this->memory);
      if (result == vk::Result::eSuccess)
      {
        break;
      }
    }
  }

  assert(this->memory);

  return this->memory;
}

void lpe::render::Chunk::Destroy()
{
  assert(this->memory);

  device.freeMemory(this->memory, nullptr);
  this->memory = nullptr;
}

vk::DeviceSize lpe::render::Chunk::GetUsage() const
{
  return this->usage;
}

void lpe::render::Chunk::SetUsage(vk::DeviceSize usage)
{
  this->usage = usage;
}

void lpe::render::Chunk::ChangeUsage(vk::DeviceSize delta)
{
  assert(usage + delta < size);
  usage += delta;
}

bool lpe::render::Chunk::HasSpaceLeft(vk::DeviceSize delta) const
{
  return usage + delta < size;
}
