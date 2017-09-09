#ifndef WINDOW_H
#define WINDOW_H

#include "stdafx.h"
#include "Instance.h"

BEGIN_LPE

class Window
{
private:
	GLFWwindow* window; // using raw pointer because glfw is an c api and std::unique_ptr wouldn't work due to incomplete type...
	uint32_t width;
	uint32_t height;
	std::string title;
	bool resizeable;
  lpe::Instance instance;
  lpe::Device device;
  lpe::SwapChain swapChain;

protected:
	virtual void Create();

public:
	Window() = default;
	Window(const Window &window) = delete;
	Window(Window &&window) = delete;
	Window operator=(const Window &window) const = delete;
	Window operator=(Window &&window) const = delete;

	Window(uint32_t width, uint32_t height, std::string title, bool resizeable = false);
	virtual ~Window();

  // TODO: add functions for imgui stuff and further methods to preinit window!
  void Create(uint32_t width, uint32_t height, std::string title, bool resizeable = false);

	bool IsOpen() const;

	void Render();
};

END_LPE

#endif