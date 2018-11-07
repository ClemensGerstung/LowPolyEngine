#ifndef LOWPOLYENGINE_MEMORYMANAGEMENT_HPP
#define LOWPOLYENGINE_MEMORYMANAGEMENT_HPP

#include <vulkan/vulkan.hpp>

namespace lpe
{
namespace rendering
{
namespace vulkan
{
namespace common
{
vk::Buffer CreateBuffer(vk::Device device,
                        vk::DeviceSize size,
                        vk::BufferUsageFlags usage,
                        const uint32_t *queueFamilies = nullptr,
                        uint32_t queueFamilyCount = 0);

uint32_t GetMemoryType(vk::PhysicalDeviceMemoryProperties deviceProperties,
                       vk::MemoryRequirements requirements,
                       vk::MemoryPropertyFlags properties,
                       uint32_t typeOffset = 0);

uint32_t AllocateMemory(vk::Device device,
                        vk::PhysicalDevice physicalDevice,
                        vk::MemoryRequirements requirements,
                        vk::MemoryPropertyFlags properties,
                        vk::DeviceMemory *memory);

bool CreateBufferAllocateAndBindMemory(vk::Device device,
                                       vk::PhysicalDevice physicalDevice,
                                       vk::Buffer& buffer,
                                       vk::DeviceMemory& memory,
                                       uint32_t& memoryType,
                                       vk::DeviceSize size,
                                       vk::BufferUsageFlags usage,
                                       vk::MemoryPropertyFlags properties,
                                       const uint32_t *queueFamilies = nullptr,
                                       uint32_t queueFamilyCount = 0);
}

class VulkanManager;

enum class MarkerPosition
{
  None,
  Before,
  After
};

class StackAllocator
{
private:
  std::weak_ptr<lpe::rendering::vulkan::VulkanManager> manager;
  std::weak_ptr<lpe::utils::log::ILogManager> logger;

  vk::Device device;
  vk::Buffer buffer;
  vk::DeviceMemory memory;
  vk::DeviceSize size;
  vk::DeviceSize offset;
  vk::DeviceSize marker;
  vk::MemoryPropertyFlags properties;
  uint32_t memoryType;

public:
  StackAllocator() = default;

  ~StackAllocator() = default;

  void Create(std::shared_ptr<lpe::rendering::vulkan::VulkanManager> manager,
              vk::DeviceSize size,
              vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
              vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferSrc);
  void Destroy();

  vk::DeviceSize Push(void *data,
                      vk::DeviceSize size,
                      MarkerPosition pos = MarkerPosition::None);
  vk::DeviceSize Pop(bool complete = false);

  void SetMarker(vk::DeviceSize offset);
  void RemoveMarker();

  bool Fits(vk::DeviceSize size) const;
};


} // vulkan
} // rendering
} // lpe

#endif //LOWPOLYENGINE_MEMORYMANAGEMENT_HPP
