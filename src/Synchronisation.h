#pragma once

#include "DeviceLevelObject.h"

namespace lpe
{
  namespace vulkan
  {
    class Semaphore : public DeviceLevelObject
    {
    private:
      vk::Semaphore semaphore;
    public:
      Semaphore() = default;
      Semaphore(const Semaphore& other);
      Semaphore(Semaphore&& other) noexcept;
      Semaphore& operator=(const Semaphore& other);
      Semaphore& operator=(Semaphore&& other) noexcept;

      Semaphore(Instance* instance,
                PhysicalDevice* physicalDevice,
                Device* device);

      ~Semaphore();
    };

    class Fence : public DeviceLevelObject
    {
    private:
      vk::Fence fence;
    public:
      Fence() = default;
      Fence(const Fence& other);
      Fence(Fence&& other) noexcept;
      Fence& operator=(const Fence& other);
      Fence& operator=(Fence&& other) noexcept;

      Fence(Instance* instance,
            PhysicalDevice* physicalDevice,
            Device* device,
            bool signaled = false);

      ~Fence();

      void Wait(uint64_t timeout) const;
      void Reset();
    };
  }
}
