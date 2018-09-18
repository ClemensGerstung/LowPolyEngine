#pragma once
#include <vulkan/vulkan.hpp>
#include <map>

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
      vk::DeviceSize usage;
    public:
      Chunk();
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

      operator bool() const;
      bool operator!() const;
      operator vk::DeviceMemory() const;
    };

    class VkMemoryManagement
    {
    private:
      vk::DeviceSize defaultSize;
      vk::Device device;
      std::vector<Chunk> chunks;
      std::map<vk::Image, Chunk*> imageMappings;
      std::map<vk::Buffer, Chunk*> bufferMappings;

      Chunk& GetCurrentChunk(vk::PhysicalDevice physicalDevice,
                             vk::MemoryRequirements requirements,
                             vk::MemoryPropertyFlagBits properties);
    public:
      VkMemoryManagement() = default;
      ~VkMemoryManagement() = default;

      void Create(vk::Device device,
                  vk::DeviceSize defaultSize);

      void Bind(vk::PhysicalDevice physicalDevice,
                vk::Image image,
                vk::MemoryPropertyFlagBits properties = vk::MemoryPropertyFlagBits::eDeviceLocal);
      void Bind(vk::PhysicalDevice physicalDevice,
                vk::Buffer buffer,
                vk::MemoryPropertyFlagBits properties = vk::MemoryPropertyFlagBits::eDeviceLocal);

      void Free(vk::Image image);
      void Free(vk::Buffer buffer);
    };
  }
}
