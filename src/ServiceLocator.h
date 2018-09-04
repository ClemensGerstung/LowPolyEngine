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
    namespace log
    {
      class ILogManager;
      class NullLogManager;
    }

    class IResourceManager;
    class NullResourceManager;
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

    template <typename TServiceImpl,
              typename = typename std::enable_if<std::is_base_of<TService,
                                                                 typename std::decay<TServiceImpl>::type>::value>::type,
              typename = typename std::enable_if<std::is_base_of<TService,
                                                                 typename std::decay<TNullService>::type>::value>::type>
    void Provide(TServiceImpl* service);
    void ProvideDirect(bool init = true);
  };

  class ServiceLocator
  {
  public:
#ifdef ENABLE_TEST_MANAGER
    static Locator<TestManager> Test;
#endif

    static Locator<utils::IResourceManager, utils::NullResourceManager> ResourceManager;
    static Locator<utils::log::ILogManager, utils::log::NullLogManager> LogManager;
  };

#ifdef ENABLE_TEST_MANAGER
  Locator<TestManager> ServiceLocator::Test = {};
#endif
}

#include "ServiceLocator.inl"
