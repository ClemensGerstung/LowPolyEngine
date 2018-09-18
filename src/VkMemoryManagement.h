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
      vk::Device device;
      vk::DeviceSize usage = 0;
    public:
      Chunk() = default;
      Chunk(const Chunk& other) = default;
      Chunk(Chunk&& other) noexcept = default;
      Chunk& operator=(const Chunk& other) = default;
      Chunk& operator=(Chunk&& other) noexcept = default;
      ~Chunk();

      const vk::DeviceMemory& Create(vk::Device device,
                                     vk::PhysicalDevice physicalDevice,
                                     vk::MemoryRequirements requirements,
                                     vk::MemoryPropertyFlagBits properties);
      void Destroy();

      vk::DeviceSize GetUsage() const;
      void SetUsage(vk::DeviceSize usage);
      void ChangeUsage(vk::DeviceSize delta);
      bool HasSpaceLeft(vk::DeviceSize delta) const;
    };

    class VkMemoryManagement
    {
    private:
      vk::DeviceSize defaultSize;
      vk::Device device;
    public:
      VkMemoryManagement();
      ~VkMemoryManagement();

      void Map(vk::Image image);
      void Map(vk::Buffer buffer);
    };
  }
}
