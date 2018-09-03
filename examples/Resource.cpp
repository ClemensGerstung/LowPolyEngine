#include "../src/lpe.h"
#include <iostream>

void OnResourceLoaded(const char* stream,
                      uint64_t size);

int main()
{
  {
    lpe::utils::ResourceManager mgr;
    mgr.Initialize();

    lpe::ServiceLocator::ResourceManager.Provide<lpe::utils::ResourceManager>(&mgr);
  }

  auto mgr = lpe::ServiceLocator::ResourceManager.Get();
  auto ptr = mgr.lock();

  assert(ptr);

  auto resource = ptr->Load("models/cube.ply",
                            OnResourceLoaded);

  auto rptr = resource.lock();
  assert(rptr);

  lpe::utils::Resource r;
  r.Load("models/cube.ply",
         OnResourceLoaded);

  return 0;
}

void OnResourceLoaded(const char* stream,
                      uint64_t size)
{
  std::vector<char> data;
  data.insert(std::begin(data),
              stream,
              (stream + size));
}
