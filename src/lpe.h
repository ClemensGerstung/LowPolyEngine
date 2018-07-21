#pragma once

#include "Event.h"
#include "Window.h"


namespace lpe
{
  namespace utils
  {
    class Logger;
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

    static ServiceLocator& Default;

    // each manager will be added manually until I figure out how to store them all in a std::vector or some other datastructure.
    // also unique_ptr because utils::Logger is an incomplete type which will lead so some compile errors
    // also cannot include utils.h because of cyclic includes
    std::unique_ptr<utils::Logger> Logger;
  };
}