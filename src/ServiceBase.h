#pragma once

namespace lpe
{
  class ServiceBase
  {
  public:
    ServiceBase() = default;
    ServiceBase(const ServiceBase& other) = delete;
    ServiceBase(ServiceBase&& other) noexcept = delete;
    ServiceBase& operator=(const ServiceBase& other) = delete;
    ServiceBase& operator=(ServiceBase&& other) noexcept = delete;
    virtual ~ServiceBase() = default;

    virtual void Initialize() = 0;
    virtual void Close() = 0;
  };

  class NullService : public ServiceBase
  {
  public:
    NullService() = default;
    NullService(const NullService& other) = delete;
    NullService(NullService&& other) noexcept = delete;
    NullService& operator=(const NullService& other) = delete;
    NullService& operator=(NullService&& other) noexcept = delete;
    ~NullService() override = default;

    void Initialize() override
    {
    }

    void Close() override
    {
    }
  };
}