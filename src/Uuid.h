#pragma once
#include <cstdint>
#include <array>

namespace lpe
{
  namespace utils
  {
    class Uuid
    {
    private:
      std::array<uint8_t, 16> data;
      Uuid() = default;
    public:
      static Uuid GetNew();

      /**
       * \brief Generates the Uuid String in the format {XXXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXXXXXXXX}. This Method is quite expensive! Use it only for debugging!
       * \return Uuid String
       */
      std::string ToString() const;

      bool operator==(const Uuid& other) const;
      bool operator!=(const Uuid& other) const;
    };
  }
}
