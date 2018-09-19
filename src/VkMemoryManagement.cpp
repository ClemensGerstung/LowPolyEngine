#include "VkMemoryManagement.h"
#include "ServiceLocator.h"

lpe::render::Chunk::Chunk()
  : memory(nullptr),
    size(VK_WHOLE_SIZE),
    alignment(VK_WHOLE_SIZE),
    properties(),
    device(nullptr),
    usage(0)
{
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

bool lpe::render::Chunk::HasSpaceLeft(vk::DeviceSize delta) const
{
  bool result = false;

  for (auto&& range : freed)
  {
    if ((range.End - range.Begin) >= delta)
    {
      result = true;
      break;
    }
  }

  return (usage + delta < size) && result;
}

lpe::render::Chunk::operator bool() const
{
  return memory;
}

bool lpe::render::Chunk::operator!() const
{
  return !memory;
}

lpe::render::Chunk::operator vk::DeviceMemory() const
{
  return memory;
}

vk::DeviceSize lpe::render::Chunk::MoveMarker(vk::DeviceSize size)
{
  std::vector<Range>::iterator min;
  vk::DeviceSize minDelta = VK_WHOLE_SIZE;

  std::vector<Range> items;
  items.emplace_back(usage,
                     this->size);
  items.insert(std::end(items),
               std::begin(freed),
               std::end(freed));

  for (auto iter = std::begin(items); iter != std::end(items); ++iter)
  {
    vk::DeviceSize delta = iter->End - iter->Begin;
    if (delta >= size && delta < minDelta)
    {
      minDelta = delta;
      min = iter;
    }
  }

  assert(minDelta != VK_WHOLE_SIZE);

  allocations.emplace_back(min->Begin,
                           (min->Begin + size));

  if (min->Begin == usage)
  {
    usage += size;
  }

  auto find = std::find_if(std::begin(freed),
                           std::end(freed),
                           [begin = min->Begin](const Range& range)
                           {
                             return range.Begin == begin;
                           });

  if (find != std::end(freed))
  {
    if ((find->End - find->Begin) == size)
    {
      freed.erase(find);
    }
    else
    {
      find->Begin = find->Begin + size;
    }
  }

  return min->Begin;
}

void lpe::render::Chunk::FreeMarker(vk::DeviceSize offset)
{
  auto iter = std::find_if(std::begin(allocations),
                           std::end(allocations),
                           [offset = offset](const Range& r)
                           {
                             return r.Begin == offset;
                           });

  assert(iter != std::end(allocations));

  auto begin = std::find_if(std::begin(freed),
                            std::end(freed),
                            [begin = iter->Begin](const Range& r)
                            {
                              return begin == r.End;
                            });

  auto end = std::find_if(std::begin(freed),
                          std::end(freed),
                          [end = iter->End](const Range& r)
                          {
                            return end == r.Begin;
                          });

  if (begin != std::end(freed))
  {
    begin->End = iter->End;
  }

  if (end != std::end(freed))
  {
    end->Begin = iter->Begin;
  }

  bool erase = begin != std::end(freed) && begin->End == usage;
  if (erase)
  {
    usage = begin->Begin;
  }

  if ((begin == std::end(freed)) &&
    (end == std::end(freed)))
  {
    freed.emplace_back(iter->Begin,
                       iter->End);
  }

  if (erase)
  {
    freed.erase(begin);
  }

  allocations.erase(iter);
}

bool lpe::render::VkMemoryManagement::Mapping::operator<(const Mapping& other) const
{
  if (buffer && other.buffer)
  {
    return buffer < other.buffer;
  }

  if (image && other.image)
  {
    return image < other.image;
  }

  return false;
}

lpe::render::Chunk& lpe::render::VkMemoryManagement::GetCurrentChunk(vk::PhysicalDevice physicalDevice,
                                                                     vk::MemoryRequirements requirements,
                                                                     vk::MemoryPropertyFlagBits properties)
{
  Chunk& chunk = chunks.back();
  if (!chunk)
  {
    if (!chunk.HasSpaceLeft(requirements.size))
    {
      chunks.emplace_back();
      chunk = chunks.back();
    }

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

vk::DeviceSize lpe::render::VkMemoryManagement::Bind(vk::PhysicalDevice physicalDevice,
                                                     vk::Image image,
                                                     vk::MemoryPropertyFlagBits properties)
{
  assert(device);

  auto requirements = device.getImageMemoryRequirements(image);
  auto chunk = GetCurrentChunk(physicalDevice,
                               requirements,
                               properties);
  auto offset = chunk.GetUsage();
  device.bindImageMemory(image,
                         chunk,
                         offset);
  //chunk.ChangeUsage(requirements.size);

  Mapping m = { image };
  ChunkOffset co = { &chunk, offset };

  mappings.insert(std::make_pair(m,
                                 co));

  return offset;
}

vk::DeviceSize lpe::render::VkMemoryManagement::Bind(vk::PhysicalDevice physicalDevice,
                                                     vk::Buffer buffer,
                                                     vk::MemoryPropertyFlagBits properties)
{
  assert(device);

  auto requirements = device.getBufferMemoryRequirements(buffer);
  auto chunk = GetCurrentChunk(physicalDevice,
                               requirements,
                               properties);
  auto offset = chunk.GetUsage();
  device.bindBufferMemory(buffer,
                          chunk,
                          offset);
  //chunk.ChangeUsage(requirements.size);

  // fu c++...
  Mapping m = { nullptr };
  m.buffer = buffer;

  ChunkOffset co = { &chunk, offset };

  mappings.insert(std::make_pair(m,
                                 co));

  return offset;
}

void lpe::render::VkMemoryManagement::Free(vk::Image image)
{
  Mapping m = { image };
  ChunkOffset chunkOffset = mappings.at(m);

  auto requirements = device.getImageMemoryRequirements(image);
}
