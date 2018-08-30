#pragma once

#include <map>
#include <memory>
#include "Uuid.h"
#include <cassert>

namespace lpe
{
  class ServiceLocator;

  class ManagerBase
  {
  public:
    ManagerBase() = default;
    virtual ~ManagerBase() = default;

    virtual void Initialize() = 0;
    virtual void Close() = 0;

    virtual utils::Uuid GetUuid() const = 0;
  };

  class ServiceLocator
  {
  private:
    std::map<utils::Uuid, std::shared_ptr<ManagerBase>> manager;
  public:
    template <typename TManager,
              typename = typename std::enable_if<std::is_base_of<ManagerBase,
                                                                 typename std::decay<TManager>::type>::value>::type>
    void Register();

    template <typename TManager,
              typename = typename std::enable_if<std::is_base_of<ManagerBase,
                                                                 typename std::decay<TManager>::type>::value>::type>
    std::weak_ptr<TManager> Resolve();
  };

  template <typename TManager, typename>
  void ServiceLocator::Register()
  {
    TManager manager;
    manager.Initialize();

    this->manager.insert(std::pair<utils::Uuid, std::shared_ptr<ManagerBase>>(manager.GetUuid(),
                                                                              std::make_shared<TManager>(manager)));
  }

  template <typename TManager, typename>
  std::weak_ptr<TManager> ServiceLocator::Resolve()
  {
    TManager mgr;
    auto uuid = mgr.GetUuid();

    auto found = this->manager.find(uuid);
    assert(found != std::end(this->manager));

    return std::make_shared<TManager>(*reinterpret_cast<TManager*>(found->second.get()));
  }
}
