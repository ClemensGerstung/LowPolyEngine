#include "lpe.h"
#include "utils.h"
#include "VulkanManager.h"

lpe::ServiceLocator lpe::ServiceLocator::Default = {};

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
  Logger = std::make_shared<utils::Logger>();
  Logger->Initialize();

  Renderer = std::make_shared<vulkan::VulkanManager>();
  Renderer->Initialize();
}

void lpe::ServiceLocator::Destroy()
{
  Renderer->Destroy();
  Logger->Destroy();
}