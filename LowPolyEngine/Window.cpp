#include "Window.h"
#include <string>
#include "stdafx.h"
#include "SwapChain.h"
#include <future>

lpe::Window::Window(const uint32_t width, const uint32_t height, std::string&& title, const bool resizeable)
{
	Window::InitWindow(width, height, title, resizeable);
}

lpe::Window::Window(const uint32_t width, const uint32_t height, const std::string& title, const bool resizeable)
{
	Window::InitWindow(width, height, title, resizeable);
}

lpe::Window::~Window()
{
	// TODO: device.waitIdle();
	glfwDestroyWindow(window);
}

void lpe::Window::InitWindow(const uint32_t width, const uint32_t height, const std::string& title, const bool resizeable)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, resizeable ? GLFW_TRUE : GLFW_FALSE);

	this->window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	this->width = width;
	this->height = height;

	this->swapChain.Init(title, window, width, height);	// will be overridden in another class with multi GPU support...

	this->pipeline.Create(swapChain.GetPhysicalDevice(), swapChain.GetLogicalDevice(), swapChain.GetSwapChainImageFormat(), swapChain.GetSwapChainExtent());

	this->commands.CreateCommandPool(swapChain.GetLogicalDevice(), swapChain.FindQueueFamilies().graphicsFamily);

	auto depthFormat = pipeline.FindDepthFormat();

	depthImageView = {swapChain.GetPhysicalDevice(), swapChain.GetLogicalDevice(), true};
	depthImageView.Create(swapChain.GetSwapChainExtent().width,
	                      swapChain.GetSwapChainExtent().height,
	                      depthFormat,
	                      vk::ImageTiling::eOptimal,
	                      vk::ImageUsageFlagBits::eDepthStencilAttachment,
	                      vk::MemoryPropertyFlagBits::eDeviceLocal,
	                      vk::ImageAspectFlagBits::eDepth);
	depthImageView.TransitionImageLayout(commands,
	                                     swapChain.GetGraphicsQueue(),
	                                     depthFormat,
	                                     vk::ImageLayout::eUndefined,
	                                     vk::ImageLayout::eDepthStencilAttachmentOptimal);

	swapChain.CreateFrameBuffers(depthImageView, pipeline.GetRenderPass());
}

bool lpe::Window::IsOpen() const
{
	return !glfwWindowShouldClose(window);
}

void lpe::Window::Render()
{
	glfwPollEvents();

	// TODO: UpdateUniformBuffers();
	// TODO: DrawFrame();

	// TODO: calculate FPS?
}

uint32_t lpe::Window::GetHeight() const
{
	return height;
}

uint32_t lpe::Window::GetWidth() const
{
	return width;
}
