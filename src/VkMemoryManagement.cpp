#include "VkMemoryManagement.h"


lpe::render::Chunk::Chunk(const Chunk& other)
  : memory(other.memory),
    size(other.size),
    alignment(other.alignment),
    properties(other.properties)
{
}

lpe::render::Chunk::Chunk(Chunk&& other) noexcept
  : memory(std::move(other.memory)),
    size(other.size),
    alignment(other.alignment),
    properties(other.properties)
{
}

lpe::render::Chunk& lpe::render::Chunk::operator=(const Chunk& other)
{
  if (this == &other) return *this;
  memory = other.memory;
  size = other.size;
  alignment = other.alignment;
  properties = other.properties;
  return *this;
}

lpe::render::Chunk& lpe::render::Chunk::operator=(Chunk&& other) noexcept
{
  if (this == &other) return *this;
  memory = std::move(other.memory);
  size = other.size;
  alignment = other.alignment;
  properties = other.properties;
  return *this;
}

lpe::render::Chunk::~Chunk()
{
  assert(!memory);
}

const vk::DeviceMemory& lpe::render::Chunk::Create(vk::Device device,
                                                   vk::PhysicalDevice physicalDevice,
                                                   vk::MemoryRequirements requirements,
                                                   vk::MemoryPropertyFlagBits properties)
{
  this->memory = nullptr;
  this->size = requirements.size;
  this->alignment = requirements.alignment;
  this->properties = properties;

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

void lpe::render::Chunk::Destroy(vk::Device device)
{
  assert(this->memory);

  device.freeMemory(this->memory, nullptr);
  this->memory = nullptr;
}
