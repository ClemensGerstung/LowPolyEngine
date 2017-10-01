#include <iostream>
#include "lpe.h"
#include "Window.h"
#include "Model.h"

int main()
{
  lpe::Window window;
  try
  {
    window.Create(800, 600, "Hello Vulkan", false);
    //lpe::Model m = window.AddModel("models/tree.ply");

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

