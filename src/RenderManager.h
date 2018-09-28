#pragma once

#include "ServiceBase.h"
#include "VkMemoryManagement.h"

namespace lpe
{
  namespace render
  {
    class IRenderManager : public ServiceBase
    {
    public:
      virtual ~IRenderManager() = default;

      virtual void Draw() = 0;
    };

    class VulkanManager : public IRenderManager
    {
      VkMemoryManagement dynamicMemory;
      VkStackAllocator stackAllocator;
    };
  }
}
