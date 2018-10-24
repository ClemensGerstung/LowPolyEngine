#include "../include/lpe/lpe.h"
#include <iostream>

int main()
{
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

  std::cout << uuid1.HashCode() << std::endl
    << uuid2.HashCode() << std::endl;

  return 0;
}