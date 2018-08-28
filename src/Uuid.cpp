#include "Uuid.h"
#include "MersenneTwister.h"
#include <sstream>
#include <iomanip>

lpe::utils::Uuid lpe::utils::Uuid::GetNew()
{
  Uuid uuid;
  lpe::random::MT19937 mt;

  for (unsigned char& i : uuid.data)
  {
    i = (mt.Next() % 0xFF) & 0xFF;
  }

  return uuid;
}

std::string lpe::utils::Uuid::ToString() const
{
  std::stringstream stream;

  stream << '{' << 
    std::setfill('0') << std::setw(2) << 
    std::hex << static_cast<uint32_t>(data[0]) << 
    std::setw(2) << static_cast<uint32_t>(data[1]) << 
    std::setw(2) << static_cast<uint32_t>(data[2]) << 
    std::setw(2) << static_cast<uint32_t>(data[3]) << '-' << 
    std::setw(2) << static_cast<uint32_t>(data[4]) << 
    std::setw(2) << static_cast<uint32_t>(data[5]) << '-' << 
    std::setw(2) << static_cast<uint32_t>(data[6]) << 
    std::setw(2) << static_cast<uint32_t>(data[7]) << '-' << 
    std::setw(2) << static_cast<uint32_t>(data[8]) << 
    std::setw(2) << static_cast<uint32_t>(data[9]) << '-' << 
    std::setw(2) << static_cast<uint32_t>(data[10]) << 
    std::setw(2) << static_cast<uint32_t>(data[11]) <<
    std::setw(2) << static_cast<uint32_t>(data[12]) << 
    std::setw(2) << static_cast<uint32_t>(data[13]) << 
    std::setw(2) << static_cast<uint32_t>(data[14]) << 
    std::setw(2) << static_cast<uint32_t>(data[15]) << '}';

  return stream.str();
}

bool lpe::utils::Uuid::operator==(const Uuid& other) const
{
  return data == other.data;
}

bool lpe::utils::Uuid::operator!=(const Uuid& other) const
{
  return data != other.data;
}