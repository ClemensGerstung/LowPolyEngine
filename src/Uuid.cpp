#include "Uuid.h"
#include "MersenneTwister.h"
#include <sstream>
#include <iomanip>

lpe::utils::Uuid::Uuid()
{
  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = 0;
  data[10] = 0;
  data[11] = 0;
  data[12] = 0;
  data[13] = 0;
  data[14] = 0;
  data[15] = 0;
}

lpe::utils::Uuid::Uuid(const Uuid& other)
{
  this->data = other.data;
}

lpe::utils::Uuid::Uuid(Uuid&& other) noexcept
{
  this->data = other.data;
}

lpe::utils::Uuid& lpe::utils::Uuid::operator=(Uuid&& other) noexcept
{
  this->data = other.data;
  return *this;
}

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

size_t lpe::utils::Uuid::HashCode() const
{
  size_t a = (static_cast<size_t>(data[0]) << 56ull) |
    (static_cast<size_t>(data[1]) << 48ull) |
    (static_cast<size_t>(data[2]) << 40ull) |
    (static_cast<size_t>(data[3]) << 32ull) |
    (static_cast<size_t>(data[4]) << 24) |
    (static_cast<size_t>(data[5]) << 16) |
    (static_cast<size_t>(data[6]) << 8) |
    (static_cast<size_t>(data[7]));
  size_t b = (static_cast<size_t>(data[8]) << 56ull) |
    (static_cast<size_t>(data[9]) << 48ull) |
    (static_cast<size_t>(data[10]) << 40ull) |
    (static_cast<size_t>(data[11]) << 32ull) |
    (static_cast<size_t>(data[12]) << 24) |
    (static_cast<size_t>(data[13]) << 16) |
    (static_cast<size_t>(data[14]) << 8) |
    (static_cast<size_t>(data[15]));

  return a ^ b;
}

bool lpe::utils::Uuid::operator==(const Uuid& other) const
{
  return data[0] == other.data[0] &&
         data[1] == other.data[1] &&
         data[2] == other.data[2] &&
         data[3] == other.data[3] &&
         data[4] == other.data[4] &&
         data[5] == other.data[5] &&
         data[6] == other.data[6] &&
         data[7] == other.data[7] &&
         data[8] == other.data[8] &&
         data[9] == other.data[9] &&
         data[10] == other.data[10] &&
         data[11] == other.data[11] &&
         data[12] == other.data[12] &&
         data[13] == other.data[13] &&
         data[14] == other.data[14] &&
         data[15] == other.data[15];
}

bool lpe::utils::Uuid::operator!=(const Uuid& other) const
{
  return !this->operator==(other);
}

bool lpe::utils::Uuid::operator<(const Uuid& other) const
{
  return HashCode() < other.HashCode();
}

lpe::utils::Uuid::operator bool() const
{
  return (data[0] + data[1] + data[2] + data[3] + 
          data[4] + data[5] + data[6] + data[7] + 
          data[8] + data[9] + data[10] + data[11] + 
          data[12] + data[13] + data[14] + data[15]) != 0;
}

size_t std::hash<lpe::utils::Uuid>::operator()(const lpe::utils::Uuid& uuid) const
{
  return uuid.HashCode();
}
