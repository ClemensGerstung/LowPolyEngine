#define ENABLE_TEST_MANAGER
#define TEST_MANAGER ITestManager
#define NULL_TEST_MANAGER NullTestManager

#include "../src/lpe.h"
#include <iostream>

class TEST_MANAGER : public lpe::ServiceBase
{
  
};

class NULL_TEST_MANAGER : public TEST_MANAGER
{
  
};

class TestManager : public TEST_MANAGER
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
    TestManager manager;
    manager.Initialize();
    lpe::ServiceLocator::Test.Provide<>(&manager);
  }

  {
    auto mgnr = lpe::ServiceLocator::Test.Get();
    auto ptr = mgnr.lock();
    assert(ptr);


  }

  return 0;
}
