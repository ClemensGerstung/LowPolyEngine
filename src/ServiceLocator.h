#pragma once

#include <map>
#include <memory>
#include "Uuid.h"
#include <cassert>

class TestManager;

namespace lpe
{
  class ManagerBase
  {
  public:
    ManagerBase() = default;
    virtual ~ManagerBase() = default;

    virtual void Initialize() = 0;
    virtual void Close() = 0;
  };

  template<typename TManager>
  class Locator
  {
  protected:
    std::shared_ptr<TManager> manager;

  public:
    std::weak_ptr<TManager> Get() const;
    void Provide(TManager* mgr);
    void Provide(bool init = true);
  };

  template <typename TManager>
  std::weak_ptr<TManager> Locator<TManager>::Get() const
  {
    return manager;
  }

  template <typename TManager>
  void Locator<TManager>::Provide(TManager* mgr)
  {
    this->manager.reset(mgr);
  }

  template <typename TManager>
  void Locator<TManager>::Provide(bool init)
  {
    this->manager = std::make_shared<TManager>();

    if(init &&
      typeid(ManagerBase).before(typeid(TManager)))
    {
      reinterpret_cast<ManagerBase*>(this->manager.get())->Initialize();
    }
  }

  class ServiceLocator
  {
  public:
    static Locator<TestManager> Test;
  };

  Locator<TestManager> ServiceLocator::Test = {};
}
