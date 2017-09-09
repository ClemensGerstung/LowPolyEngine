#include "Window.h"

void lpe::Window::Create()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, resizeable ? GLFW_TRUE : GLFW_FALSE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

  instance.Create(title);
  device = instance.CreateDevice(window);
  swapChain = device.CreateSwapChain(width, height);
}

lpe::Window::Window(uint32_t width, uint32_t height, std::string title, bool resizeable)
	: width(width),
	  height(height),
	  title(title),
	  resizeable(resizeable)
{
	Create();
}

lpe::Window::~Window()
{
	if(window)
	{
		glfwDestroyWindow(window);
	}
}

void lpe::Window::Create(uint32_t width, uint32_t height, std::string title, bool resizeable)
{
  if(window)
    throw std::runtime_error("Window was already created. Consider using the default constructor if you want to use this function!");

  this->width = width;
  this->height = height;
  this->title = title;
  this->resizeable = resizeable;
  Create();
}

bool lpe::Window::IsOpen() const
{
  if (!window)
    throw std::runtime_error("Tried to get window state of not existing window");

	return !glfwWindowShouldClose(window);
}

void lpe::Window::Render()
{
  if (!window)
    throw std::runtime_error("Cannot render on a window if there is no window!");

	glfwPollEvents();


}
