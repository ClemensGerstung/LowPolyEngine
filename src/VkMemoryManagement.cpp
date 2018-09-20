#include "VkMemoryManagement.h"
#include "ServiceLocator.h"


int32_t lpe::render::utils::GetMemoryType(vk::PhysicalDeviceMemoryProperties deviceProperties,
                                          vk::MemoryRequirements requirements,
                                          vk::MemoryPropertyFlagBits properties,
                                          uint32_t typeOffset)
{
  for (uint32_t type = typeOffset; type < deviceProperties.memoryTypeCount; ++type)
  {
    if ((requirements.memoryTypeBits & (1 << type)) &&
      (deviceProperties.memoryTypes[type].propertyFlags & properties) == properties)
    {
      return static_cast<int32_t>(type);
    }
  }

  return -1;
}

void lpe::render::utils::AllocateDeviceMemory(vk::Device device,
                                              vk::PhysicalDevice physicalDevice,
                                              vk::MemoryRequirements requirements,
                                              vk::MemoryPropertyFlagBits properties,
                                              vk::DeviceSize size,
                                              vk::DeviceMemory* memory,
                                              uint32_t* type)
{
  *type = 0;
  vk::Result result = vk::Result::eSuccess;
  do
  {
    *type = GetMemoryType(physicalDevice.getMemoryProperties(),
                          requirements,
                          properties,
                          *type);

    vk::MemoryAllocateInfo allocateInfo =
    {
      size + (size % requirements.alignment),
      *type
    };

    result = device.allocateMemory(&allocateInfo,
                                   nullptr,
                                   memory);
  }
  while (result != vk::Result::eSuccess);
}

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

  uint32_t type;
  utils::AllocateDeviceMemory(device,
                              physicalDevice,
                              requirements,
                              properties,
                              this->size,
                              &this->memory,
                              &type);

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
  vk::DeviceSize usage = 0;
  for (auto iter = std::begin(allocations); iter != std::end(allocations); ++iter)
  {
    usage += iter->End - iter->Begin;
  }

  return usage;
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

  return (usage + delta <= size) || result;
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
                                                     vk::Image& image,
                                                     vk::MemoryPropertyFlagBits properties)
{
  assert(device);

  auto requirements = device.getImageMemoryRequirements(image);
  auto chunk = GetCurrentChunk(physicalDevice,
                               requirements,
                               properties);

  auto offset = chunk.MoveMarker(requirements.size);
  device.bindImageMemory(image,
                         chunk,
                         offset);

  Mapping m = { image };
  ChunkOffset co = { &chunk, offset };

  mappings.insert(std::make_pair(m,
                                 co));

  return offset;
}

vk::DeviceSize lpe::render::VkMemoryManagement::Bind(vk::PhysicalDevice physicalDevice,
                                                     vk::Buffer& buffer,
                                                     vk::MemoryPropertyFlagBits properties)
{
  assert(device);

  auto requirements = device.getBufferMemoryRequirements(buffer);
  auto chunk = GetCurrentChunk(physicalDevice,
                               requirements,
                               properties);

  auto offset = chunk.MoveMarker(requirements.size);
  device.bindBufferMemory(buffer,
                          chunk,
                          offset);

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

  Chunk* chunk = chunkOffset.chunk;
  chunk->FreeMarker(chunkOffset.offset);

  if (chunk->GetUsage() == 0)
  {
    chunk->Destroy();
  }
}

void lpe::render::VkMemoryManagement::Free(vk::Buffer buffer)
{
  // fu c++...
  Mapping m = { nullptr };
  m.buffer = buffer;

  ChunkOffset chunkOffset = mappings.at(m);

  Chunk* chunk = chunkOffset.chunk;
  chunk->FreeMarker(chunkOffset.offset);

  if (chunk->GetUsage() == 0)
  {
    chunk->Destroy();
  }
}

void lpe::render::VkStackAllocator::Create(vk::Device device,
                                           vk::PhysicalDevice physicalDevice,
                                           vk::DeviceSize size,
                                           vk::MemoryPropertyFlagBits properties,
                                           vk::MemoryRequirements requirements)
{
  assert(device);
  assert(!memory);
  assert(size > 0);

  this->device = device;
  this->size = size;
  this->memory = nullptr;
  this->properties = properties;
  this->offset = 0;
  this->marker = 0;

  utils::AllocateDeviceMemory(device,
                              physicalDevice,
                              requirements,
                              properties,
                              this->size,
                              &this->memory,
                              &memoryType);

  assert(memory);
}

void lpe::render::VkStackAllocator::Destroy()
{
  assert(memory);
  assert(device);
  assert(offset == 0);

  device.freeMemory(memory,
                    nullptr);
  memory = nullptr;
}

vk::DeviceSize lpe::render::VkStackAllocator::Push(vk::PhysicalDevice physicalDevice,
                                                   vk::Buffer& buffer,
                                                   MarkerPosition pos)
{
  auto requirements = device.getBufferMemoryRequirements(buffer);
  auto deviceProperties = physicalDevice.getMemoryProperties();
  vk::DeviceSize begin = offset;

  assert((begin + requirements.size) < size);

  offset = offset + requirements.size;

  int32_t type = utils::GetMemoryType(deviceProperties,
                                      requirements,
                                      properties);

  assert(type >= 0 &&
    ((static_cast<uint32_t>(type)) == memoryType));

  device.bindBufferMemory(buffer,
                          memory,
                          begin);

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

vk::DeviceSize lpe::render::VkStackAllocator::Pop(bool marker)
{
  offset = marker ? this->marker : 0;

  return offset;
}

void lpe::render::VkStackAllocator::SetMarker(vk::DeviceSize offset)
{
  assert(offset < this->offset);
  marker = offset;
}

void lpe::render::VkStackAllocator::RemoveMarker()
{
  marker = 0;
}
