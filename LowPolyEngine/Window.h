#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <string>

namespace lpe
{
	class Window
	{
	private:
		GLFWwindow* window;	// using raw pointer because glfw is an c api and std::unique_ptr wouldn't work due to incomplete type...

		void InitWindow(const uint32_t width, const uint32_t height, const std::string& title);

	public:
		Window() = delete;

		Window(const uint32_t width, const uint32_t height, std::string&& title);
		Window(const uint32_t width, const uint32_t height, const std::string& title);

		Window(const Window& window) = delete;
		Window(Window&& window) = delete;

		~Window();

		Window operator=(const Window& window) const = delete;
		Window operator=(Window&& window) const = delete;
	};
}

#endif