#include "Window.h"
#include <string>

lpe::Window::Window(const uint32_t width, const uint32_t height, std::string&& title)
{
	InitWindow(width, height, title);
}

lpe::Window::Window(const uint32_t width, const uint32_t height, const std::string& title)
{
	InitWindow(width, height, title);
}

lpe::Window::~Window()
{
	glfwDestroyWindow(window);
}

void lpe::Window::InitWindow(const uint32_t width, const uint32_t height, const std::string& title)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}
