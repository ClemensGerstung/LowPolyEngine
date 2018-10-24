#include "../include/lpe/lpe.h"
#include <iostream>

void OnResourceLoaded(const uint8_t* stream,
                      uint64_t size);

int main()
{
  {
    lpe::utils::ResourceManager mgr;
    mgr.Initialize();

    lpe::ServiceLocator::ResourceManager.Provide<lpe::utils::ResourceManager>(&mgr);
  }

  auto mgr = lpe::ServiceLocator::ResourceManager.Get()
                                                 .lock();

  assert(mgr);

  auto resource = mgr->Load("models/tree.ply",
                            OnResourceLoaded).lock();

  assert(resource);

  lpe::utils::Resource r;
  r.Load("models/cube.ply",
         OnResourceLoaded);

  return 0;
}

void OnResourceLoaded(const uint8_t* stream,
                      uint64_t size)
{
  std::cout << "loaded: " << size << "b" << std::endl;
}
