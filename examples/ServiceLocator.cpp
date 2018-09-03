#define ENABLE_TEST_MANAGER

#include "../src/lpe.h"
#include <iostream>

class TestManager : public lpe::ServiceBase
{
private:
  uint32_t value;

public:
  TestManager() : value(0) { }
  ~TestManager() override = default;
  void Initialize() override
  {
    lpe::random::MT19937 mt;
    value = mt.Next() % 1000;
    std::cout << value << std::endl;
  }
  void Close() override
  {

  }
};

int main()
{
  {
    //TestManager manager;
    //manager.Initialize();
    lpe::ServiceLocator::Test.ProvideDirect();
  }

  {
    auto mgnr = lpe::ServiceLocator::Test.Get();
    auto ptr = mgnr.lock();
    assert(ptr);


  }

  return 0;
}
