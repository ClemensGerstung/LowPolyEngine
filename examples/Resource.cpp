#include "../src/lpe.h"
#include <iostream>

void OnResourceLoaded(const std::fstream& stream);

class TestManager : public lpe::ManagerBase
{
private:
  uint32_t value;

public:
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
    lpe::ServiceLocator::Test.Provide();
  }

  {
    auto mgnr = lpe::ServiceLocator::Test.Get();
    auto ptr = mgnr.lock();
    assert(ptr);

    
  }

  //lpe::utils::Resource r;
  //r.Load("models/cube.ply", 
  //       lpe::utils::ResourceType::Mesh, 
  //       OnResourceLoaded);

  return 0;
}

void OnResourceLoaded(const std::fstream& stream)
{

}