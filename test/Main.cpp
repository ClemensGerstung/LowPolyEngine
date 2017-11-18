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

  uint32_t instances = 20;

  for (uint32_t i = 0; i < instances; ++i)
  {
    for (uint32_t j = 0; j < instances; ++j)
    {
      object.GetInstance(i * instances + j)->SetPosition({ i, j, 0 });
    }
  }


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

      

      window.Render();
    }
  }
  catch (std::runtime_error e)
  {
    std::cerr << e.what() << std::endl;

    return EXIT_FAILURE;
  }

  return 0;
}