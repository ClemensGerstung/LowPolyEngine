#ifndef BUFFERMEMORY_H
#define BUFFERMEMORY_H

#include "lpe.h"
#include <map>

BEGIN_LPE
  class BufferMemory
  {
  private:
    std::unique_ptr<vk::Device, helper::Deleter<vk::Device>> device;
    vk::PhysicalDevice physicalDevice;

    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vk::DeviceSize size;
    void* mapped;
    vk::DescriptorBufferInfo descriptor;

    vk::BufferUsageFlags usageFlags;
    vk::MemoryPropertyFlags memoryPropertyFlags;

    std::map<uint32_t, vk::DeviceSize> offsets;
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

    BufferMemory(vk::Device* device,
                 vk::PhysicalDevice physicalDevice,
                 std::initializer_list<std::pair<uint32_t, vk::DeviceSize>> pairs);

    vk::Result Map(vk::DeviceSize size = VK_WHOLE_SIZE,
                   vk::DeviceSize offset = 0);
    void Unmap();
    void Bind(vk::DeviceSize offset = 0);
    vk::DescriptorBufferInfo SetupDescriptor(vk::DeviceSize size = VK_WHOLE_SIZE,
                                             vk::DeviceSize offset = 0);
    void Write(void* data,
               vk::DeviceSize size,
               bool map = true);
    void WriteStaged(void* data,
                     vk::DeviceSize size,
                     BufferMemory& buffer,
                     vk::DeviceSize offset,
                     vk::CommandBuffer command);
    vk::Result Flush(vk::DeviceSize size = VK_WHOLE_SIZE,
                     vk::DeviceSize offset = 0);
    vk::Result Invalidate(vk::DeviceSize size = VK_WHOLE_SIZE,
                          vk::DeviceSize offset = 0);
    void Destroy();


    vk::Buffer GetBuffer() const;
    vk::DescriptorBufferInfo GetDescriptor() const;
    vk::BufferUsageFlags GetUsageFlags() const;
    vk::MemoryPropertyFlags GetMemoryPropertyFlags() const;
  };

END_LPE

#endif
