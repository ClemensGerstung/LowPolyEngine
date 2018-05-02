#pragma once

#include <functional>
#include <vector>

namespace lpe
{
  namespace event
  {
    class EventArgs
    {
    public:
      EventArgs() = default;
      EventArgs(const EventArgs& other) = default;
      EventArgs(EventArgs&& other) = default;
      EventArgs& operator=(const EventArgs& other) = default;
      EventArgs& operator=(EventArgs&& other) = default;
      virtual ~EventArgs() = default;

      const static EventArgs Empty;
    };

    template<typename TSender, 
             typename TArgs = EventArgs,
             typename = typename std::enable_if<std::is_base_of<EventArgs, 
                                                                typename std::decay<TArgs>::type>::value>::type>  // this shitty boilerplate is just to ensure that TArgs is derived from EventArgs...
    class Event
    {
    public:
      using Callback = std::function<void(const TSender&, const TArgs&)>;
    private:
      std::vector<Callback> callbacks;
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

      void operator+=(Callback callback)
      {
        callbacks.push_back(callback);
      }

      //template<class TLambda>
      //void operator+=(TLambda callback) const
      //{
      //  const_cast<std::vector<Callback>&>(callbacks).emplace_back(callback);
      //}

      void operator()(const TSender& sender, const TArgs& args) const
      {
        Invoke(sender, args);
      }

      void Invoke(const TSender& sender, const TArgs& args) const
      {
        for (auto iter = std::begin(callbacks); iter != std::end(callbacks); ++iter)
        {
          (*iter)(sender, args);
        }
      }
    };
  }
}