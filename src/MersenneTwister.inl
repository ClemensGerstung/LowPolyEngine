//#include "MersenneTwister.h"
#include <cassert>
#include <ctime>

template <typename TType, uint16_t w, uint16_t n, uint32_t m, uint32_t r, uint32_t a, uint32_t u, uint32_t d, uint32_t s, uint32_t b, uint32_t t, uint32_t c, uint32_t l, uint32_t f>
void lpe::random::MersenneTwister<TType, w, n, m, r, a, u, d, s, b, t, c, l, f>::Twist()
{
  for (uint16_t i = 0; i < n; i++)
  {
    TType x = (mt[i] & UPPER) + (mt[(i + 1) % n] & LOWER);
    TType xA = x >> 1;

    if (x % 2 != 0)
    {
      xA ^= a;
    }

    mt[i] = mt[(i + m) % n] ^ xA;
  }

  index = 0;
}

template<typename TType, uint16_t w, uint16_t n, uint32_t m, uint32_t r, uint32_t a, uint32_t u, uint32_t d, uint32_t s, uint32_t b, uint32_t t, uint32_t c, uint32_t l, uint32_t f>
inline lpe::random::MersenneTwister<TType, w, n, m, r, a, u, d, s, b, t, c, l, f>::MersenneTwister()
{
  std::time_t timeStamp = std::time(nullptr);
  this->SetSeed(static_cast<TType>(timeStamp));
}

template <typename TType, uint16_t w, uint16_t n, uint32_t m, uint32_t r, uint32_t a, uint32_t u, uint32_t d, uint32_t s, uint32_t b, uint32_t t, uint32_t c, uint32_t l, uint32_t f>
void lpe::random::MersenneTwister<TType, w, n, m, r, a, u, d, s, b, t, c, l, f>::SetSeed(const TType seed)
{
  index = n;
  mt[0] = seed;

  for (uint16_t i = 1; i < n; i++)
  {
    mt[i] = (f * (mt[i - 1] ^ (mt[i - 1] >> (w - 2))) + 1);
  }
}

template <typename TType, uint16_t w, uint16_t n, uint32_t m, uint32_t r, uint32_t a, uint32_t u, uint32_t d, uint32_t s, uint32_t b, uint32_t t, uint32_t c, uint32_t l, uint32_t f>
TType lpe::random::MersenneTwister<TType, w, n, m, r, a, u, d, s, b, t, c, l, f>::Next()
{
  if (index >= n)
  {
    assert(index <= n);

    this->Twist();
  }

  TType x = mt[index];
  x ^= ((x >> u) & d);
  x ^= ((x << s) & b);
  x ^= ((x << t) & c);
  x ^= (x >> l);

  index = index + 1;

  return x;
}
