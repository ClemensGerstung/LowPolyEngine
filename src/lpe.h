#pragma once

#include "Event.h"
#include "Window.h"

#ifndef VK_MAKE_VERSION
#define LPE_MAKE_VERSION(major, minor, patch) \
    (((major) << 22) | ((minor) << 12) | (patch))
#else
#define LPE_MAKE_VERSION(major, minor, patch) VK_MAKE_VERSION(major, minor, patch)
#endif

namespace lpe
{
  namespace utils
  {
    class Logger;
  }

  namespace vulkan
  {
    class VulkanManager;
  }

  constexpr uint32_t LpeVersion = VK_MAKE_VERSION(0, 0, 1);

  void Initialize();

  void Close();

  class LpeObject
  {
  public:
    virtual void Initialize() = 0;
    virtual void Destroy() = 0;
  };

  class ServiceLocator : public LpeObject
  {
  public:
    ServiceLocator() = default;
    ~ServiceLocator() = default;

    void Initialize() override;
    void Destroy() override;

    static ServiceLocator Default;

    // each manager will be added manually until I figure out how to store them all in a std::vector or some other datastructure.
    // also unique_ptr because utils::Logger is an incomplete type which will lead so some compile errors
    // also cannot include utils.h because of cyclic includes
    std::shared_ptr<utils::Logger> Logger;
    std::shared_ptr<vulkan::VulkanManager> Renderer;
  };

  class Guid
  {
  private:
    uint64_t pt1;
    uint32_t pt2;
    uint32_t pt3;
    uint32_t pt4;
    uint32_t pt5;
    uint64_t pt6;

    Guid() = default;

  public:
    static Guid NewGuid();
    std::string ToString() const;
  };
}

#include "utils.h"
#include "VulkanManager.h"
