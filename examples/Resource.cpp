#include "../src/lpe.h"
#include <iostream>

void OnResourceLoaded(const std::fstream& stream);

class TestManager : public lpe::ManagerBase
{
private:
  static lpe::utils::Uuid uuid;

public:
  ~TestManager() override = default;
  void Initialize() override
  {
    if(!uuid)
    {
      uuid = lpe::utils::Uuid::GetNew();
    }
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

  //lpe::utils::Resource r;
  //r.Load("models/cube.ply", 
  //       lpe::utils::ResourceType::Mesh, 
  //       OnResourceLoaded);

  return 0;
}

void OnResourceLoaded(const std::fstream& stream)
{

}