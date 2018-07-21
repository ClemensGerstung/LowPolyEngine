#include <iostream>
#include "../src/lpe.h"

int main()
{
  lpe::window::Window window("Hello World",
                             1920,
                             1080,
                             true);

  window.KeyPressed += [](const lpe::window::Window& window,
                          const lpe::window::WindowKeyPressedEventArgs& e)
  {
    std::cout << static_cast<uint32_t>(e.GetKey()) << std::endl;
  };

  while (window.IsOpen())
  {
    window.Render();
  }

  return 0;
}
