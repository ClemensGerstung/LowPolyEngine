#include "lpe.h"
#include "utils.h"

void lpe::Initialize()
{
  ServiceLocator::Default.Initialize();

}

void lpe::Close()
{

  ServiceLocator::Default.Destroy();
}

void lpe::ServiceLocator::Initialize()
{
  Logger = std::make_unique<utils::Logger>();
  Logger->Initialize();
}

void lpe::ServiceLocator::Destroy()
{
  Logger->Destroy();
  Logger.release();
}