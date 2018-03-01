#ifndef EVENT_H
#define EVENT_H

#include "lpe.h"
#include <functional>

BEGIN_LPE
  template <typename TSenderType, typename... TArgs>
  class Event
  {
  private:
    std::vector<std::function<void(const TSenderType&, TArgs&&...)>> callbacks;
  public:
    Event() = default;

    Event(const Event& other)
    {
      callbacks = { other.callbacks };
    }

    Event(Event&& other) noexcept
    {
      callbacks = std::move(other.callbacks);
    }

    Event& operator=(const Event& other)
    {
      callbacks = { other.callbacks };
      return *this;
    }

    Event& operator=(Event&& other) noexcept
    {
      callbacks = std::move(other.callbacks);
      return *this;
    }

    ~Event() = default;

    void operator+=(std::function<void(const TSenderType&, TArgs&&...)> callback) const
    {
      callbacks.push_back(callback);
    }

    template<class TLambda>
    void operator+=(TLambda callback) const
    {
      const_cast<std::vector<std::function<void(const TSenderType&, TArgs&&...)>>&>(callbacks).emplace_back(callback);
    }

    void operator()(const TSenderType& sender, TArgs&&... args) const
    {
      Invoke(sender, args...);
    }

    void Invoke(const TSenderType& sender, TArgs&&... args) const
    {
      for(auto iter = std::begin(callbacks); iter != std::end(callbacks); ++iter)
      {
        (*iter)(sender, args...);
      }
    }
  };

END_LPE

#endif
