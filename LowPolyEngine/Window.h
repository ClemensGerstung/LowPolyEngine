#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN

#include <string>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "VulkanInstance.h"

namespace lpe
{
	class Window
	{
	private:
		GLFWwindow* window;	// using raw pointer because glfw is an c api and std::unique_ptr wouldn't work due to incomplete type...
		std::unique_ptr<Vulkan> vulkan;


		void InitWindow(const uint32_t width, const uint32_t height, const std::string& title, const bool resizeable = true);

	public:
		Window() = delete;

		Window(const uint32_t width, const uint32_t height, std::string&& title, const bool resizeable = true);
		Window(const uint32_t width, const uint32_t height, const std::string& title, const bool resizeable = true);

		Window(const Window& window) = delete;
		Window(Window&& window) = delete;

		~Window();

		Window operator=(const Window& window) const = delete;
		Window operator=(Window&& window) const = delete;

		bool IsOpen() const;
		void Render();
	};
}

#endif