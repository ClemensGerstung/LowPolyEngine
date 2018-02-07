#ifndef BUFFERMEMORY_H
#define BUFFERMEMORY_H

#include "lpe.h"
#include <map>

BEGIN_LPE
  template <uint32_t bufferCount>
  struct BufferMemoryCreateInfo
  {
    const uint32_t count = bufferCount;
    vk::MemoryPropertyFlags propertyFlags;
    std::array<vk::BufferUsageFlags, bufferCount> usages;
    std::array<std::map<uint32_t, vk::DeviceSize>, bufferCount> offsets;
    std::array<uint32_t, bufferCount> ids;
  };

  class BufferMemory
  {
  private:
    std::unique_ptr<vk::Device, helper::Deleter<vk::Device>> device;
    vk::PhysicalDevice physicalDevice;

    std::map<uint32_t, vk::Buffer> buffers;
    vk::DeviceMemory memory;
    vk::DeviceSize size;
    std::map<uint32_t, vk::DeviceSize> alignments;
    void* mapped;
    vk::DescriptorBufferInfo descriptor;

    std::vector<vk::BufferUsageFlags> usageFlags;
    vk::MemoryPropertyFlags memoryPropertyFlags;

    std::map<uint32_t, std::map<uint32_t, vk::DeviceSize>> offsets;

  public:
    enum class Type : uint32_t
    {
      Vertex = 0,
      Index = 1,
      Indirect = 2,
      Instance = 3,
      UBO = 4,
      Stage = 5
      // tbc
    };


    BufferMemory() = default;
    BufferMemory(const BufferMemory& other);
    BufferMemory(BufferMemory&& other) noexcept;
    BufferMemory& operator=(const BufferMemory& other);
    BufferMemory& operator=(BufferMemory&& other) noexcept;
    ~BufferMemory();

    template <uint32_t bufferCount>
    BufferMemory(vk::Device* device,
                 vk::PhysicalDevice physicalDevice,
                 BufferMemoryCreateInfo<bufferCount> createInfo);

    vk::Result Map(vk::DeviceSize size = VK_WHOLE_SIZE,
                   vk::DeviceSize offset = 0);
    void Unmap();
    void Bind(uint32_t id,
              vk::DeviceSize offset = 0);
    vk::DescriptorBufferInfo SetupDescriptor(uint32_t id,
                                             vk::DeviceSize size = VK_WHOLE_SIZE,
                                             vk::DeviceSize offset = 0);
    void Write(void* data,
               vk::DeviceSize size,
               vk::DeviceSize offset = 0,
               bool map = true);
    void WriteStaged(uint32_t sourceId,
                     uint32_t targetId,
                     void* data,
                     vk::DeviceSize size,
                     BufferMemory& buffer,
                     vk::DeviceSize offset,
                     vk::CommandBuffer command);
    vk::Result Flush(vk::DeviceSize size = VK_WHOLE_SIZE,
                     vk::DeviceSize offset = 0);
    vk::Result Invalidate(vk::DeviceSize size = VK_WHOLE_SIZE,
                          vk::DeviceSize offset = 0);
    void Destroy();

    vk::DeviceSize GetSize(uint32_t id,
                           uint32_t key);
    vk::DeviceSize GetOffset(uint32_t id,
                             uint32_t key);

    void Get(uint32_t id,
             uint32_t key,
             vk::DeviceSize* size,
             vk::DeviceSize* offset);

    vk::Buffer GetBuffer(uint32_t id) const;
    vk::DescriptorBufferInfo GetDescriptor() const;
    vk::BufferUsageFlags GetUsageFlags(uint32_t id) const;
    vk::MemoryPropertyFlags GetMemoryPropertyFlags() const;
  };

  template <uint32_t bufferCount>
  lpe::BufferMemory::BufferMemory(vk::Device* device,
                                  vk::PhysicalDevice physicalDevice,
                                  BufferMemoryCreateInfo<bufferCount> createInfo)
    : physicalDevice(physicalDevice),
      memoryPropertyFlags(createInfo.propertyFlags)
  {
    this->device.reset(device);
    size = 0;
    mapped = nullptr;

    usageFlags.insert(std::end(usageFlags),
                      std::begin(createInfo.usages),
                      std::end(createInfo.usages));

    for (uint32_t i = 0; i < createInfo.count; ++i)
    {
      uint32_t id = createInfo.ids[i];
      auto usage = usageFlags[i];
      auto offsets = createInfo.offsets[i];

      std::for_each(std::begin(offsets),
                    std::end(offsets),
                    [&size = size](const std::pair<uint32_t, vk::DeviceSize> pair) { size += pair.second; });

      vk::Buffer buffer = {};
      vk::BufferCreateInfo createInfo = { {}, size, usage, vk::SharingMode::eExclusive };
      auto result = device->createBuffer(&createInfo,
                                         nullptr,
                                         &buffer);

      helper::ThrowIfNotSuccess(result,
                                "Failed to create buffer!");

      vk::MemoryRequirements requirements = device->getBufferMemoryRequirements(buffer);
      auto memoryProperties = physicalDevice.getMemoryProperties();
      auto memoryIndex = helper::FindMemoryTypeIndex(requirements.memoryTypeBits,
                                                     memoryPropertyFlags,
                                                     memoryProperties);

      vk::MemoryAllocateInfo allocInfo = { requirements.size, memoryIndex };

      result = device->allocateMemory(&allocInfo,
                                      nullptr,
                                      &memory);
      helper::ThrowIfNotSuccess(result,
                                "Failed to allocate buffer memory!");

      Bind(id);

      buffers.insert_or_assign(id,
                               buffer);
      alignments.insert_or_assign(id,
                                  requirements.alignment);
      offsets.insert_or_assign(id,
                               offsets);
    }
  }

END_LPE

#endif
