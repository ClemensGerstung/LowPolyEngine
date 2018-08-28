#include "../src/lpe.h"
#include <iostream>

int main()
{
  lpe::random::MT19937 mt;

  for (size_t i = 0; i < 1000; i++)
  {
    std::cout << (mt.Next() % 100) << std::endl;
  }

  for (size_t i = 0; i < 1000; i++)
  {
    lpe::utils::Uuid uuid = lpe::utils::Uuid::GetNew();

    std::cout << uuid.ToString().c_str() << std::endl;
  }

  lpe::utils::Uuid uuid1 = lpe::utils::Uuid::GetNew();
  lpe::utils::Uuid uuid2 = lpe::utils::Uuid::GetNew();
  lpe::utils::Uuid uuid3 = uuid1;

  std::cout << (uuid1 == uuid2) << std::endl
    << (uuid3 == uuid1) << std::endl
    << (uuid2 == uuid3) << std::endl;

  return 0;
}