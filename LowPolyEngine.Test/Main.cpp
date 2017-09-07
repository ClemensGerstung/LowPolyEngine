#include <iostream>
#include "lpe.h"
#include "Window.h"

int main()
{
	try
	{
		lpe::Window window = { 800, 600, "Hello Vulkan", false };

		while(window.IsOpen())
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