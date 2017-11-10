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
    window.Create(1280, 720, "LowPolyEngine", false);
    //modelsRenderer.AddObject({
    //  { { 0.5, 0.5, 0.5 },{ 1, 0, 0 } },
    //  { { -0.5, 0.5, 0.5 },{ 0, 1, 0 } },
    //  { { -0.5, -0.5, 0.5 },{ 0, 0, 1 } },
    //  { { 0.5, -0.5, 0.5 },{ 1, 1, 1 } }
    //},
    //{ 0, 1, 2, 2, 3, 0 });

   
    window.AddModel("models/cube.ply");
    window.AddModel("models/tree.ply");
    window.AddModel("models/monkey.ply");
    

    std::unique_ptr<lpe::Model> tree = window.GetElement(1);
    std::unique_ptr<lpe::Model> cube = window.GetElement(0);
    std::unique_ptr<lpe::Model> mnky = window.GetElement(2);

    tree->SetPosition({ 0, 2, 0 });
    mnky->SetPosition({ 0, -2, 0 });

    auto startTime = std::chrono::high_resolution_clock::now();
    
    while (window.IsOpen())
    {
      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 2500.0f;

      tree->SetTransform(glm::rotate(glm::mat4(1.0f), time * glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
      //tree->Transform(glm::scale(glm::mat4(1.0), { 0.75, 0.75, 0.75 }));

      cube->SetTransform(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
      cube->Transform(glm::scale(glm::mat4(1.0), { 0.5, 0.5, 0.5 }));

      mnky->SetTransform(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
      mnky->Transform(glm::scale(glm::mat4(1.0), { 0.5, 0.5, 0.5 }));

      window.Render();
    }

    tree.release();
    cube.release();
    mnky.release();
  }
  catch (std::runtime_error e)
  {
    std::cerr << e.what() << std::endl;

    return EXIT_FAILURE;
  }

  return 0;
}

