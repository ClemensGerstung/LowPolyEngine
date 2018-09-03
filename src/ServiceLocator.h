#pragma once

#include <memory>
#include "Uuid.h"
#include "ServiceBase.h"
#include "ResourceManager.h"

#ifdef ENABLE_TEST_MANAGER
class TestManager;
#endif

namespace lpe
{
  namespace utils
  {
    class ResourceManager;
  }

  template <typename TService, typename TNullService = NullService>
  class Locator
  {
  protected:
    std::shared_ptr<TService> service;
    TNullService nullService;
  public:
    Locator() = default;
    Locator(const Locator& other) = delete;
    Locator(Locator&& other) noexcept = delete;
    Locator& operator=(const Locator& other) = delete;
    Locator& operator=(Locator&& other) noexcept = delete;
    ~Locator() = default;

    std::weak_ptr<TService> Get() const;

    template<typename = typename std::enable_if<std::is_base_of<TService,
                                                typename std::decay<TNullService>::type>::value>::type>
    void Provide(TService* service);
    void ProvideDirect(bool init = true);
  };

  class ServiceLocator
  {
  public:
#ifdef ENABLE_TEST_MANAGER
    static Locator<TestManager> Test;
#endif

    static Locator<utils::ResourceManager> ResourceManager;
  };

#ifdef ENABLE_TEST_MANAGER
  Locator<TestManager> ServiceLocator::Test = {};
#endif
}

#include "ServiceLocator.inl"