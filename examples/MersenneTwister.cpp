#include "../src/lpe.h"
#include <iostream>

int main()
{
  lpe::random::MT19937 mt;
  uint64_t t = 0;

  for (size_t i = 0; i < 1000; i++)
  {
    uint32_t r = mt.Next() % 100;
    t += r;
    
    std::cout << (r) << std::endl;
  }

  std::cout << (t / 1000) << std::endl;

  return 0;
}