#pragma once

#include "lpe.h"

namespace lpe
{
  namespace utils
  {
    class LogEventArgs : public event::EventArgs
    {
    private:
      std::string message;
    public:
      LogEventArgs() = default;
      ~LogEventArgs() = default;

      const std::string& GetMessage() const;
      void SetMessage(const std::string& string);
      void SetMessage(const char* message);
    };

    class Logger : public LpeObject
    {
    private:
      lpe::event::Event<Logger, LogEventArgs> output;
    public:
      Logger() = default;
      ~Logger() = default;

      void Initialize() override;
      void Destroy() override;

      void Write(std::string& message);
      void Write(const char* message);

      void AddListener(void(*func)(const Logger&, const LogEventArgs&));
    };

  }
}