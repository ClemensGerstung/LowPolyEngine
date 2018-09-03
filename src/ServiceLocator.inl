#pragma once

lpe::Locator<lpe::utils::ResourceManager> lpe::ServiceLocator::ResourceManager = {};

template <typename TService, typename TNullService>
std::weak_ptr<TService> lpe::Locator<TService, TNullService>::Get() const
{
  return service;
}

template <typename TService, typename TNullService>
template <typename>
void lpe::Locator<TService, TNullService>::Provide(TService* service)
{
  if (service == nullptr)
  {
    this->service.reset(&this->nullService);
  }
  else
  {
    this->service.reset(service);
  }
}

template <typename TService, typename TNullService>
void lpe::Locator<TService, TNullService>::ProvideDirect(bool init)
{
  this->service = std::make_shared<TService>();

  if (init &&
    typeid(ServiceBase).before(typeid(TService)))
  {
    reinterpret_cast<ServiceBase*>(this->service.get())->Initialize();
  }
}