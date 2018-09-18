#include "VkMemoryManagement.h"
#include "ServiceLocator.h"

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
  assert(memory);

  device.freeMemory(memory,
                    nullptr);
  memory = nullptr;
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

lpe::render::Chunk::operator bool() const
{
  return memory;
}

bool lpe::render::Chunk::operator!() const
{
  return !memory;
}

lpe::render::Chunk& lpe::render::VkMemoryManagement::GetCurrentChunk(vk::PhysicalDevice physicalDevice,
                                                                     vk::MemoryRequirements requirements,
                                                                     vk::MemoryPropertyFlagBits properties)
{
  Chunk& chunk = chunks.back();
  if (!chunk || !chunk.HasSpaceLeft(requirements.size))
  {
    chunks.emplace_back();
    chunk = chunks.back();

    if (requirements.size > defaultSize)
    {
      auto manager = ServiceLocator::LogManager.Get()
                                               .lock();
      if (manager)
      {
        manager->Log("Required size of image is larger than default size.");
      }
    }
    else
    {
      requirements.size = defaultSize + (defaultSize % requirements.alignment);
    }

    chunk.Create(device,
                 physicalDevice,
                 requirements,
                 properties);
  }

  return chunk;
}

void lpe::render::VkMemoryManagement::Create(vk::Device device,
                                             vk::DeviceSize defaultSize)
{
  this->device = device;
  this->defaultSize = defaultSize;

  chunks.emplace_back();
}

void lpe::render::VkMemoryManagement::Bind(vk::PhysicalDevice physicalDevice,
                                           vk::Image image,
                                           vk::MemoryPropertyFlagBits properties)
{
  auto requirements = device.getImageMemoryRequirements(image);
  auto chunk = GetCurrentChunk(physicalDevice,
                               requirements,
                               properties);
}
