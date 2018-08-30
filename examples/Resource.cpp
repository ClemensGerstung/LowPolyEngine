#include "../src/lpe.h"
#include <iostream>

void OnResourceLoaded(const std::fstream& stream);

class TestManager : public lpe::ManagerBase
{
private:
  static lpe::utils::Uuid uuid;

  uint32_t value;

public:
  ~TestManager() override = default;
  void Initialize() override
  {
    if(!uuid)
    {
      uuid = lpe::utils::Uuid::GetNew();
    }

    lpe::random::MT19937 mt;
    value = mt.Next() % 1000;
    std::cout << value << std::endl;
  }
  void Close() override
  {

  }

  lpe::utils::Uuid GetUuid() const override
  {
    return uuid;
  }
};

lpe::utils::Uuid TestManager::uuid = {};

int main()
{
  lpe::ServiceLocator locator;
  locator.Register<TestManager>();
  auto mgr = locator.Resolve<TestManager>();

  //lpe::utils::Resource r;
  //r.Load("models/cube.ply", 
  //       lpe::utils::ResourceType::Mesh, 
  //       OnResourceLoaded);

  return 0;
}

void OnResourceLoaded(const std::fstream& stream)
{

}