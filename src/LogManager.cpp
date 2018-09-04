#include "LogManager.h"
#include <iostream>

lpe::utils::log::LogEventArgs::LogEventArgs(const std::string & message)
{
  this->message = message;
}

lpe::utils::log::LogEventArgs::LogEventArgs(std::string && message)
{
  this->message = message;
}

lpe::utils::log::LogEventArgs::LogEventArgs(const LogEventArgs& other)
{
  this->message = other.message;
}

lpe::utils::log::LogEventArgs::LogEventArgs(LogEventArgs&& other) noexcept
{
  this->message = std::move(other.message);
}

lpe::utils::log::LogEventArgs& lpe::utils::log::LogEventArgs::operator=(const LogEventArgs& other)
{
  this->message = other.message;
  return *this;
}

lpe::utils::log::LogEventArgs& lpe::utils::log::LogEventArgs::operator=(LogEventArgs&& other) noexcept
{
  this->message = std::move(other.message);
  return *this;
}

std::string lpe::utils::log::LogEventArgs::GetMessage() const
{
  return this->message;
}

void lpe::utils::log::LogManager::Initialize()
{
  this->log += [](const ILogManager& sender, const LogEventArgs& e)
  {
    std::cout << e.GetMessage().c_str() << std::endl;
  };
}

void lpe::utils::log::LogManager::Close()
{
}

void lpe::utils::log::LogManager::Log(const std::string& message)
{
  this->log(*this, { message });
}

void lpe::utils::log::LogManager::Log(std::string&& message)
{
  this->log(*this, { message });
}


