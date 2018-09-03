#pragma once

lpe::Locator<lpe::utils::IResourceManager, lpe::utils::NullResourceManager> lpe::ServiceLocator::ResourceManager = {};

template <typename TService, typename TNullService>
std::weak_ptr<TService> lpe::Locator<TService, TNullService>::Get() const
{
  return service;
}

//template <typename TService, typename TNullService>
//template <typename>
//void lpe::Locator<TService, TNullService>::Provide(TService* service)
//{
//  if (service == nullptr)
//  {
//    this->service.reset(&this->nullService);
//  }
//  else
//  {
//    this->service = std::make_shared<TService>(*service);
//  }
//}

template<typename TService, typename TNullService>
template<typename TServiceImpl, typename, typename>
void lpe::Locator<TService, TNullService>::Provide(TServiceImpl * service)
{
  if (service == nullptr)
  {
    this->service.reset(&this->nullService);
  }
  else
  {
    this->service = std::make_shared<TServiceImpl>(*service);
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