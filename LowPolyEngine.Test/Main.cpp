#include <iostream>
#include "lpe.h"
#include "Window.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

int main()
{
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

    lpe::Model* m = window.AddModel("models/tree.ply");
    auto transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m->Transform(transform);

    while (window.IsOpen())
    {
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

