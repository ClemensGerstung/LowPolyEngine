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
  defaultCamera = Camera({5,10,0}, {0,0,0}, swapChain.GetExtent(), 45, 0, 100);
  commands = device.CreateCommands();
  modelsRenderer = device.CreateModelsRenderer(&commands);

  uniformBuffer = device.CreateUniformBuffer(modelsRenderer, defaultCamera);
  graphicsPipeline = device.CreatePipeline(swapChain, &uniformBuffer);
  depthImage = commands.CreateDepthImage(swapChain.GetExtent(), graphicsPipeline.FindDepthFormat());
  auto frameBuffers = swapChain.CreateFrameBuffers(graphicsPipeline.GetRenderPassRef(), &depthImage);
  commands.CreateCommandBuffers(frameBuffers, swapChain.GetExtent(), uniformBuffer.GetDynamicAlignment(), &graphicsPipeline, &modelsRenderer);
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

lpe::Model* lpe::Window::AddModel(std::string path)
{
  if (!window)
    throw std::runtime_error("Cannot add model if the window wasn't created successfull. Call Create(...) before AddModel(...)!");

  auto model = modelsRenderer.AddObject(path);
  uniformBuffer.Update(defaultCamera, modelsRenderer);
  commands.ResetCommandBuffers();
  commands.CreateCommandBuffers(swapChain.GetFramebuffers(), swapChain.GetExtent(), uniformBuffer.GetDynamicAlignment(), &graphicsPipeline, &modelsRenderer);
  
  return model;
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

  uniformBuffer.Update(defaultCamera, modelsRenderer);

  uint32_t imageIndex = -1;
  vk::SubmitInfo submitInfo = device.PrepareFrame(swapChain, &imageIndex);
  
  if(imageIndex == -1)
    return;

  submitInfo.commandBufferCount = 1;
  auto commandBuffer = commands[imageIndex];
  submitInfo.setPCommandBuffers(&commandBuffer);

  device.SubmitQueue(1, &submitInfo);

  std::vector<vk::SwapchainKHR> swapchains = { swapChain.GetSwapchain() };
  device.SubmitFrame(swapchains, &imageIndex);
}
