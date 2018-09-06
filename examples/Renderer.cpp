#include "../src/lpe.h"
#include "../src/RenderObject.h"

#define _MK_PTR(T, n) std::shared_ptr<T> n = std::make_shared<T>()


int main()
{
  lpe::Initialize();

  auto resourceManager = lpe::ServiceLocator::ResourceManager.Get()
                                                             .lock();

  assert(resourceManager);

  _MK_PTR(lpe::render::Texture, t);
  t->SetImage(resourceManager->Load("textures/lpe.jpg"));

  _MK_PTR(lpe::render::Material, m);
  m->SetAlbedo(t);



  return 0;
}
