#pragma once

template <typename TService>
std::weak_ptr<TService> lpe::Locator<TService>::Get() const
{
  return service;
}

//template <typename TService, typename TNullService>
//template <typename>
//void src::Locator<TService, TNullService>::Provide(TService* service)
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

template<typename TService>
template<typename TServiceImpl, typename>
void lpe::Locator<TService>::Provide(TServiceImpl * service)
{
  if (service == nullptr)
  {
    /*this->service.reset(&this->nullService);*/
  }
  else
  {
    this->service = std::make_shared<TServiceImpl>(*service);
  }
}