#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "SwapChain.h"

namespace lpe
{
class Window
{
	friend class SwapChain;

	GLFWwindow* window; // using raw pointer because glfw is an c api and std::unique_ptr wouldn't work due to incomplete type...
	uint32_t width;
	uint32_t height;
	SwapChain swapChain;

	void InitWindow(const uint32_t width, const uint32_t height, const std::string& title, const bool resizeable = true);

public:
	Window() = delete;

	Window(const uint32_t width,
	       const uint32_t height,
	       std::string&& title,
	       const bool resizeable = true);
	Window(const uint32_t width,
	       const uint32_t height,
	       const std::string& title,
	       const bool resizeable = true);

	Window(const Window& window) = delete;
	Window(Window&& window) = delete;

	~Window();

	Window operator=(const Window& window) const = delete;
	Window operator=(Window&& window) const = delete;

	bool IsOpen() const;
	void Render();

	uint32_t GetHeight() const;
	uint32_t GetWidth() const;
};
}

#endif
