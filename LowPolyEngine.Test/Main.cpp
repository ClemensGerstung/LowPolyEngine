#include <iostream>
#include "lpe.h"
#include "Window.h"

using namespace std;

int main()
{
	try
	{
		auto count = lpe::GetPhysicalDeviceCount();

		std::cout << count << std::endl;
	}
	catch (runtime_error e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	lpe::Window window = { 800, 600, "Hello Vulkan", false };
	
	while(window.IsOpen())
	{
		window.Render();
	}

	return 0;
}