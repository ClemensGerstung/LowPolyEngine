#pragma once
#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace render
  {
    class Chunk
    {
    private:
      vk::DeviceMemory memory;
      vk::DeviceSize size;
      vk::DeviceSize alignment;
      vk::MemoryPropertyFlagBits properties;
    public:
      Chunk() = default;
      Chunk(const Chunk& other);
      Chunk(Chunk&& other) noexcept;
      Chunk& operator=(const Chunk& other);
      Chunk& operator=(Chunk&& other) noexcept;
      ~Chunk();

      const vk::DeviceMemory& Create(vk::Device device,
                                     vk::PhysicalDevice physicalDevice,
                                     vk::MemoryRequirements requirements,
                                     vk::MemoryPropertyFlagBits properties);
      void Destroy(vk::Device device);
    };

    class VkMemoryManagement
    {
    private:
      vk::DeviceSize defaultSize;
    public:
      VkMemoryManagement();
      ~VkMemoryManagement();
    };
  }
}
