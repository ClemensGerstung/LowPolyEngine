#include "../src/lpe.h"
#include <iostream>

int main()
{
  lpe::random::MT19937 mt;

  for (size_t i = 0; i < 1000; i++)
  {
    std::cout << (mt.Next() % 100) << std::endl;
  }

  return 0;
}