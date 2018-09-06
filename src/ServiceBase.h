#pragma once

namespace lpe
{
  class ServiceBase
  {
  public:
    ServiceBase() = default;
    ServiceBase(const ServiceBase& other) = default;
    ServiceBase(ServiceBase&& other) noexcept = default;
    ServiceBase& operator=(const ServiceBase& other) = default;
    ServiceBase& operator=(ServiceBase&& other) noexcept = default;
    virtual ~ServiceBase() = default;

    virtual void Initialize() = 0;
    virtual void Close() = 0;
  };

  class NullService : public ServiceBase
  {
  public:
    NullService() = default;
    NullService(const NullService& other) = default;
    NullService(NullService&& other) noexcept = default;
    NullService& operator=(const NullService& other) = default;
    NullService& operator=(NullService&& other) noexcept = default;
    ~NullService() override = default;

    void Initialize() override
    {
    }

    void Close() override
    {
    }
  };
}