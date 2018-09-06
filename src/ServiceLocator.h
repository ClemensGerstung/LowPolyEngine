#pragma once

#include <memory>
#include "Uuid.h"
#include "ServiceBase.h"
#include "ResourceManager.h"
#include "LogManager.h"

#ifndef TEST_MANAGER
#define TEST_MANAGER ITestManager
#endif

#ifndef NULL_TEST_MANAGER
#define NULL_TEST_MANAGER NullTestManager
#endif

#ifdef ENABLE_TEST_MANAGER
class TEST_MANAGER;
class NULL_TEST_MANAGER;
#endif

namespace lpe
{
  template <typename TService, typename TNullService = NullService>
  class Locator
  {
  protected:
    std::shared_ptr<TService> service;
    //TNullService nullService;
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
  };

  class ServiceLocator
  {
  public:
#ifdef ENABLE_TEST_MANAGER
    static Locator<TEST_MANAGER, NULL_TEST_MANAGER> Test;
#endif

    static Locator<utils::IResourceManager, utils::NullResourceManager> ResourceManager;
    static Locator<utils::log::ILogManager, utils::log::NullLogManager> LogManager;
  };

#ifdef ENABLE_TEST_MANAGER
  Locator<TEST_MANAGER, NULL_TEST_MANAGER> ServiceLocator::Test = {};
#endif
}

#include "ServiceLocator.inl"
