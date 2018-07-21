#include "utils.h"

#include <iostream>

const std::string& lpe::utils::LogEventArgs::GetMessage() const
{
  return message;
}

void lpe::utils::LogEventArgs::SetMessage(const std::string & string)
{
  this->message = string;
}

void lpe::utils::LogEventArgs::SetMessage(const char * message)
{
  this->message = message;
}

void lpe::utils::Logger::Initialize()
{
  output += [](const Logger&, const LogEventArgs& e) 
  {  
    std::cout << e.GetMessage() << std::endl;
  };
}

void lpe::utils::Logger::Destroy()
{
  
}

void lpe::utils::Logger::Write(std::string & message)
{
  LogEventArgs e;
  e.SetMessage(message);
  output(*this, e);
}

void lpe::utils::Logger::Write(const char * message)
{
  LogEventArgs e;
  e.SetMessage(message);
  output(*this, e);
}

void lpe::utils::Logger::AddListener(void(*func)(const Logger &, const LogEventArgs &))
{
  output += func;
}


