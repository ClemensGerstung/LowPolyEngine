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
  lpe::Settings::GetDefault().EnableValidationLayer = true;

  lpe::RenderObject object = { "models/tree.ply", 0 };
  lpe::RenderObject monkey = { "models/monkey.ply", 0 };

  uint32_t instances = 5;

  for (uint32_t x = 0; x < instances; ++x)
  {
    for (uint32_t y = 0; y < instances; ++y)
    {
      auto instance = object.GetInstance(x * instances + y);
      instance->SetPosition({ x, y, 0 });
      instance->SetTransform(glm::scale(glm::mat4(1),
                                        { 0.75f, 0.75f, 0.75f }));

      instance = monkey.GetInstance(x * instances + y);
      instance->SetPosition({ x, y, 1 });
      instance->SetTransform(glm::scale(glm::mat4(1),
                                        { 0.5f, 0.5f, 0.5f }));
    }
  }


  lpe::Window window;
  try
  {
    window.Create(1920,
                  1080,
                  "LowPolyEngine",
                  false);
    window.AddRenderObject(&object);
    window.AddRenderObject(&monkey);

    auto startTime = std::chrono::high_resolution_clock::now();

    while (window.IsOpen())
    {
      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 2500.0f;

      for (uint32_t x = 0; x < instances; ++x)
      {
        for (uint32_t y = 0; y < instances; ++y)
        {
          auto instance = object.GetInstance(x * instances + y);
          instance->SetPosition({ x, y, 0 });
          instance->SetTransform(glm::scale(glm::mat4(1),
                                            { 0.75f, 0.75f, 0.75f }));
          instance->Transform(glm::rotate(glm::mat4(1),
                                          glm::radians(90.0f) * time,
                                          { 0, 0, 1 }));
        }
      }

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
