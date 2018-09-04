#pragma once

#include "ServiceBase.h"
#include "Event.h"

namespace lpe
{
  namespace utils
  {
    namespace log
    {
      class LogEventArgs : public EventArgs
      {
      private:
        std::string message;
      public:
        LogEventArgs(const std::string& message);
        LogEventArgs(std::string&& message);
        LogEventArgs() = default;
        LogEventArgs(const LogEventArgs& other);
        LogEventArgs(LogEventArgs&& other) noexcept;
        LogEventArgs& operator=(const LogEventArgs& other);
        LogEventArgs& operator=(LogEventArgs&& other) noexcept;


        std::string GetMessage() const;
      };

      class ILogManager : public ServiceBase
      {
      protected:
        Event<ILogManager, LogEventArgs> log;
      public:
        ILogManager() = default;
        virtual ~ILogManager() = default;

        virtual void Log(const std::string& message) = 0;
        virtual void Log(std::string&& message) = 0;
      };

      class LogManager : public ILogManager
      {
      public:
        LogManager() = default;
        ~LogManager() override = default;

        void Initialize() override;
        void Close() override;
        void Log(const std::string& message) override;
        void Log(std::string&& message) override;
      };

      class NullLogManager : public ILogManager
      {
      public:
        NullLogManager() = default;
        ~NullLogManager() override = default;

        void Initialize() override {  }
        void Close() override {  }
        
        void Log(const std::string& message) override {  }
        void Log(std::string&& message) override {  }
      };
    }
  }
}