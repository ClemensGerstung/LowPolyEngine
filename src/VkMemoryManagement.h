#pragma once
#include <vulkan/vulkan.hpp>
#include <map>

namespace lpe
{
  namespace render
  {
    namespace utils
    {
      inline bool CheckMemoryType(uint32_t types,
                                  uint32_t type,
                                  vk::MemoryType memoryTypes[32],
                                  vk::MemoryPropertyFlagBits properties);
      inline void AllocateDeviceMemory(vk::Device device,
                                       vk::PhysicalDevice physicalDevice,
                                       vk::MemoryRequirements requirements,
                                       vk::MemoryPropertyFlagBits properties,
                                       vk::DeviceSize size,
                                       vk::DeviceMemory* memory);
    }

    class Chunk
    {
    private:
      struct Range
      {
        vk::DeviceSize Begin;
        vk::DeviceSize End;

        Range() = default;

        Range(vk::DeviceSize begin,
              vk::DeviceSize end)
          : Begin(begin),
            End(end)
        {
        }
      };

      vk::DeviceMemory memory;
      vk::DeviceSize size;
      vk::DeviceSize alignment;
      vk::MemoryPropertyFlagBits properties;
      vk::Device device;
      vk::DeviceSize usage;
      std::vector<Range> allocations;
      std::vector<Range> freed;
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
      bool HasSpaceLeft(vk::DeviceSize delta) const;

      operator bool() const;
      bool operator!() const;
      operator vk::DeviceMemory() const;

      vk::DeviceSize MoveMarker(vk::DeviceSize size);
      void FreeMarker(vk::DeviceSize offset);
    };

    class VkMemoryManagement
    {
    private:
      union Mapping
      {
        vk::Image image;
        vk::Buffer buffer;

        bool operator<(const Mapping& other) const;
      };

      struct ChunkOffset
      {
        Chunk* chunk;
        vk::DeviceSize offset;

        ChunkOffset() = default;
        ChunkOffset(const ChunkOffset& other) = default;
        ChunkOffset(ChunkOffset&& other) noexcept = default;
        ChunkOffset& operator=(const ChunkOffset& other) = default;
        ChunkOffset& operator=(ChunkOffset&& other) noexcept = default;
        ~ChunkOffset() = default;
      };

      vk::DeviceSize defaultSize;
      vk::Device device;
      std::vector<Chunk> chunks;
      std::map<Mapping, ChunkOffset> mappings;

      Chunk& GetCurrentChunk(vk::PhysicalDevice physicalDevice,
                             vk::MemoryRequirements requirements,
                             vk::MemoryPropertyFlagBits properties);
    public:
      VkMemoryManagement() = default;
      ~VkMemoryManagement() = default;

      void Create(vk::Device device,
                  vk::DeviceSize defaultSize);

      vk::DeviceSize Bind(vk::PhysicalDevice physicalDevice,
                          vk::Image& image,
                          vk::MemoryPropertyFlagBits properties = vk::MemoryPropertyFlagBits::eDeviceLocal);
      vk::DeviceSize Bind(vk::PhysicalDevice physicalDevice,
                          vk::Buffer& buffer,
                          vk::MemoryPropertyFlagBits properties = vk::MemoryPropertyFlagBits::eDeviceLocal);

      void Free(vk::Image image);
      void Free(vk::Buffer buffer);
    };

    enum class MarkerPosition
    {
      None,
      Before,
      After
    };

    class VkStackAllocator
    {
    private:
      vk::DeviceSize size;
      vk::DeviceSize offset;
      vk::DeviceSize marker;
      vk::DeviceMemory memory;
      vk::Device device;
      uint32_t memoryType;
    public:
      VkStackAllocator() = default;
      VkStackAllocator(const VkStackAllocator& other) = default;
      VkStackAllocator(VkStackAllocator&& other) noexcept = default;
      VkStackAllocator& operator=(const VkStackAllocator& other) = default;
      VkStackAllocator& operator=(VkStackAllocator&& other) noexcept = default;
      ~VkStackAllocator() = default;

      void Create(vk::Device device,
                  vk::PhysicalDevice physicalDevice,
                  vk::DeviceSize size,
                  vk::MemoryPropertyFlagBits properties,
                  vk::MemoryRequirements requirements);
      void Destroy();

      vk::DeviceSize Push(vk::Buffer& buffer, MarkerPosition pos = MarkerPosition::None);
      vk::DeviceSize Push(vk::Image& image, MarkerPosition pos = MarkerPosition::None);
      vk::DeviceSize Pop(bool marker);

      void SetMarker(vk::DeviceSize offset);
      void RemoveMarker();
    };
  }
}
