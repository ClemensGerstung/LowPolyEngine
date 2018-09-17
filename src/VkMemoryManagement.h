#pragma once
#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace render
  {
    class Chunk
    {
      vk::DeviceMemory memory;
      vk::DeviceSize size;
    };

    class VkMemoryManagement
    {
    private:
      
    public:
      VkMemoryManagement();
      ~VkMemoryManagement();
    };
  }
}
