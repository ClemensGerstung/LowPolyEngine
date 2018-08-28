#pragma once

#include <cstdint>

namespace lpe
{
  namespace random
  {

    template<typename TType, 
             uint16_t w, 
             uint16_t n, 
             uint32_t m, 
             uint32_t r, 
             uint32_t a, 
             uint32_t u, 
             uint32_t d,
             uint32_t s,
             uint32_t b, 
             uint32_t t, 
             uint32_t c,
             uint32_t l,
             uint32_t f>
    class MersenneTwister
    {
    private:
      const uint64_t LOWER = (1ull << r) - 1;
      const uint64_t UPPER = (1ull << r);

      uint16_t index;
      TType mt[n];

      void Twist();

    public:
      MersenneTwister();
      ~MersenneTwister() = default;

      void SetSeed(const TType seed);
      TType Next();
    };

    using MT19937 = MersenneTwister<uint32_t, 32, 624, 397, 31, 0x9908B0DF, 11, 0xFFFFFFFF, 7, 0x9D2C5680, 15, 0xEFC60000, 18, 1812433253>;
  }
}

#include "MersenneTwister.inl"