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

    std::map<uint32_t, vk::BufferUsageFlags> usageFlags;
    vk::MemoryPropertyFlags memoryPropertyFlags;

    std::map<uint32_t, std::map<uint32_t, vk::DeviceSize>> offsets;

  public:
    struct Type
    {
      static const uint32_t Vertex = 0;
      static const uint32_t Index = 1;
      static const uint32_t Indirect = 2;
      static const uint32_t Instance = 3;
      static const uint32_t UBO = 4;
      static const uint32_t Stage = 5;
      static const uint32_t Storage = 6;
      // tbc
    };


    BufferMemory() = default;
    BufferMemory(const BufferMemory& other);
    BufferMemory(BufferMemory&& other) noexcept;
    BufferMemory& operator=(const BufferMemory& other);
    BufferMemory& operator=(BufferMemory&& other) noexcept;
    ~BufferMemory();

    template <uint32_t bufferCount = 1>
    BufferMemory(vk::Device* device,
                 vk::PhysicalDevice physicalDevice,
                 BufferMemoryCreateInfo<bufferCount> createInfo);

    template <uint32_t bufferCount = 1>
    void Recreate(BufferMemoryCreateInfo<bufferCount> createInfo);

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

    std::map<uint32_t, vk::DeviceSize> GetOffsets(uint32_t id);

    vk::Buffer GetBuffer(uint32_t id) const;
    vk::DescriptorBufferInfo GetDescriptor() const;
    vk::BufferUsageFlags GetUsageFlags(uint32_t id) const;
    vk::MemoryPropertyFlags GetMemoryPropertyFlags() const;
  };

  template <uint32_t bufferCount>
  lpe::BufferMemory::BufferMemory(vk::Device* device,
                                  vk::PhysicalDevice physicalDevice,
                                  BufferMemoryCreateInfo<bufferCount> createInfo)
    : physicalDevice(physicalDevice)
  {
    this->device.reset(device);
    this->Recreate(createInfo);
  }

  template <uint32_t bufferCount>
  void BufferMemory::Recreate(BufferMemoryCreateInfo<bufferCount> createInfo)
  {
    this->Destroy();

    memoryPropertyFlags = createInfo.propertyFlags;
    size = 0;
    mapped = nullptr;

    for (uint32_t i = 0; i < createInfo.count; ++i)
    {
      uint32_t id = createInfo.ids[i];
      auto usage = usageFlags[i];
      auto offsets = createInfo.offsets[i];
      vk::DeviceSize bufferOffset = size;

      std::for_each(std::begin(offsets),
                    std::end(offsets),
                    [&size = size](const std::pair<uint32_t, vk::DeviceSize>& pair) { size += pair.second; });

      vk::Buffer buffer = {};
      vk::BufferCreateInfo bufferCreateInfo = { {}, size, usage, vk::SharingMode::eExclusive };
      auto result = device->createBuffer(&bufferCreateInfo,
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

      usageFlags.insert(std::make_pair(id,
                                       usage));
      buffers.insert(std::make_pair(id,
                                    buffer));
      alignments.insert(std::make_pair(id,
                                       requirements.alignment));
      this->offsets.insert(std::make_pair(id,
                                          offsets));

      Bind(id,
           bufferOffset);
    }
  }

END_LPE

#endif
