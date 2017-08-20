#include <iostream>
#include <Window.h>

using namespace std;

int main()
{
	lpe::Window window = { 800, 600, "Hello Vulkan", false };
	
	while(window.IsOpen())
	{
		window.Render();
	}

	return 0;
}