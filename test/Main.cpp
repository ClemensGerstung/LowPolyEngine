#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include "lpe.h"
#include "Window.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

int main()
{
  lpe::settings.EnableValidationLayer = true;

  lpe::Window window;
  try
  {
    window.Create(800, 600, "Hello Vulkan", false);
    //modelsRenderer.AddObject({
    //  { { 0.5, 0.5, 0.5 },{ 1, 0, 0 } },
    //  { { -0.5, 0.5, 0.5 },{ 0, 1, 0 } },
    //  { { -0.5, -0.5, 0.5 },{ 0, 0, 1 } },
    //  { { 0.5, -0.5, 0.5 },{ 1, 1, 1 } }
    //},
    //{ 0, 1, 2, 2, 3, 0 });

    window.AddModel("models/tree.ply")->SetPosition({2, 0, 0});
    //lpe::Model* m = window.AddModel("models/tree.ply");
    //lpe::Model* m = window.AddModel("models/cube.ply");
    lpe::Model* m = window.AddModel("models/monkey.ply");
    //m->SetPosition({ 2, 0, 0 });
    //m->SetPosition({ 1, 1, 1 });
    auto startTime = std::chrono::high_resolution_clock::now();
    
    while (window.IsOpen())
    {
      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 10000.0f;

      auto transform = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      m->SetTransform(transform);
      //m->SetTransform(glm::mat4(1.0f));
      m->Transform(glm::scale(glm::mat4(1.0), { 0.75, 0.75, 0.75 }));

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

