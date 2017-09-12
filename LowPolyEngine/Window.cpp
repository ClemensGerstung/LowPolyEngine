#include "Window.h"
#include "UniformBufferObject.h"

void lpe::Window::Create()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, resizeable ? GLFW_TRUE : GLFW_FALSE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

  instance.Create(title);
  device = instance.CreateDevice(window);
  swapChain = device.CreateSwapChain(width, height);
  defaultCamera = Camera({2,2,2}, {0,0,0}, swapChain.GetExtent(), 60, 0, 10);
  commands = device.CreateCommands();
  depthImage = commands.CreateDepthImage(swapChain.GetExtent(), vk::Format::eUndefined);
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

lpe::Camera lpe::Window::CreateCamera(glm::vec3 position, glm::vec3 lookAt, float fov, float near, float far) const
{
  // TODO: check creation done!

  return Camera(position, lookAt, swapChain.GetExtent(), fov, near, far);
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
