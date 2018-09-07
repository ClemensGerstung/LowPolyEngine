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
    public:
      Uuid();
      Uuid(const Uuid& other);
      Uuid(Uuid&& other) noexcept;
      Uuid& operator=(const Uuid& other) = default;
      Uuid& operator=(Uuid&& other) noexcept;
      ~Uuid() = default;

      static Uuid GetNew();

      /**
       * \brief Generates the Uuid String in the format {XXXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXXXXXXXX}. This Method is quite expensive! Use it only for debugging!
       * \return Uuid String
       */
      std::string ToString() const;
      size_t HashCode() const;

      bool operator==(const Uuid& other) const;
      bool operator!=(const Uuid& other) const;

      bool operator<(const Uuid& other) const;

      operator bool() const;
    };
  }
}

namespace std
{
  template<> 
  struct hash<lpe::utils::Uuid>
  {
    size_t operator()(const lpe::utils::Uuid& uuid) const;
  };
}
