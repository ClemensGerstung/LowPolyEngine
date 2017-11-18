#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include "lpe.h"
#include "Window.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "RenderObject.h"

int main()
{
  lpe::settings.EnableValidationLayer = true;

  lpe::RenderObject object = { "models/tree.ply", 0 };
  auto instance1 = object.GetInstance(0);
  auto instance2 = object.GetInstance(1);
  auto instance3 = object.GetInstance(2);

  instance1->SetPosition({ 1, 0, 0 });
  instance3->SetPosition({ -1, 0, 0 });

  lpe::Window window;
  try
  {
    window.Create(1280, 720, "LowPolyEngine", false);
    window.AddRenderObject(&object);

    auto startTime = std::chrono::high_resolution_clock::now();
    
    while (window.IsOpen())
    {
      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 2500.0f;

      instance1->SetTransform(glm::rotate(glm::mat4(1.0), glm::radians(90.0f) * time, { 0, 0, 1 }));

      window.Render();
    }

    instance1.release();
    instance2.release();
    instance3.release();
  }
  catch (std::runtime_error e)
  {
    std::cerr << e.what() << std::endl;

    return EXIT_FAILURE;
  }

  return 0;
}