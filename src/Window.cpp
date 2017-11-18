#include "../include/Window.h"
#include "../include/UniformBufferObject.h"

#include <glm/ext.hpp>

void lpe::Window::Create()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, resizeable ? GLFW_TRUE : GLFW_FALSE);
	
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, KeyInputCallback);
  glfwSetMouseButtonCallback(window, MouseInputCallback);
  glfwSetCursorPosCallback(window, MouseMoveCallback);

  instance.Create(title);
  device = instance.CreateDevice(window);
  swapChain = device.CreateSwapChain(width, height);
  defaultCamera = { {3,0,0}, {0,0,0}, swapChain.GetExtent(), 110, 0.1f, 256 };
  commands = device.CreateCommands();
  modelsRenderer = device.CreateModelsRenderer(&commands);

  uniformBuffer = device.CreateUniformBuffer(modelsRenderer, defaultCamera, commands);
  uniformBuffer.SetLightPosition({ 2, 2, 2 });
  renderPass = device.CreateRenderPass(swapChain.GetImageFormat());
  graphicsPipeline = device.CreatePipeline(swapChain, renderPass, &uniformBuffer);
  depthImage = commands.CreateDepthImage(swapChain.GetExtent(), device.FindDepthFormat());
  
  auto frameBuffers = swapChain.CreateFrameBuffers(renderPass, &depthImage);
  commands.CreateCommandBuffers(frameBuffers, swapChain.GetExtent(), renderPass, graphicsPipeline, modelsRenderer, uniformBuffer);
}

void lpe::Window::KeyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  lpe::Window* pointer = reinterpret_cast<lpe::Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
	{
		pointer->defaultCamera.ChangeFoV(-5);
	}
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
	{
		pointer->defaultCamera.ChangeFoV(5);
	}


  if(key == GLFW_KEY_W && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Move({ -0.05, 0, 0 });
  }
  if (key == GLFW_KEY_S && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Move({ 0.05, 0, 0 });
  }
  if (key == GLFW_KEY_A && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Move({ 0, -0.05, 0 });
  }
  if (key == GLFW_KEY_D && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Move({ 0, 0.05, 0 });
  }
  if (key == GLFW_KEY_Z && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Move({ 0, 0, 0.05 });
  }
  if (key == GLFW_KEY_X && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Move({ 0, 0, -0.05 });
  }

  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Rotate(-15, {0, 0, 1});
  }
  if (key == GLFW_KEY_E && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Rotate(15, { 0, 0, 1 });
  }
  if (key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Rotate(-15, { 1, 0, 0 });
  }
  if (key == GLFW_KEY_F && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Rotate(15, { 1, 0, 0 });
  }
  if (key == GLFW_KEY_C && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Rotate(-15, { 0, 1, 0 });
  }
  if (key == GLFW_KEY_V && action == GLFW_PRESS)
  {
    pointer->defaultCamera.Rotate(15, { 0, 1, 0 });
  }

  std::cout << glm::to_string(pointer->defaultCamera.GetPosition()) << " -> " << glm::to_string(pointer->defaultCamera.GetLookAt()) << " -> " << pointer->defaultCamera.GetFoV() << std::endl;
}

void lpe::Window::MouseInputCallback(GLFWwindow* window, int button, int action, int mods)
{
  lpe::Window* pointer = reinterpret_cast<lpe::Window*>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS)
  {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    pointer->mousepos = { xpos, ypos };

    switch (button)
    {
    case GLFW_MOUSE_BUTTON_RIGHT:
      pointer->mouseState = MouseState::rightButtonPressed;
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      pointer->mouseState = MouseState::middleButtonPressed;
      break;
    case GLFW_MOUSE_BUTTON_LEFT:
      pointer->mouseState = MouseState::leftButtonPressed;
      break;
    default: 
      // TODO: do something?
      break;
    }
  }
  
  if (action == GLFW_RELEASE)
  {
    pointer->mouseState = MouseState::released;
  }
}

void lpe::Window::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
  lpe::Window* pointer = reinterpret_cast<lpe::Window*>(glfwGetWindowUserPointer(window));

  auto delta = (pointer->mousepos - glm::vec2(xpos, ypos)) / 2.5f;

  switch (pointer->mouseState)
  {
  case MouseState::rightButtonPressed:  // zoom
    pointer->defaultCamera.Move({ -delta.y / 50, 0, 0 });
    break;
  case MouseState::middleButtonPressed: // rotate
    pointer->defaultCamera.Rotate(delta.x, { 0, 0, 1 });
    pointer->defaultCamera.Rotate(-delta.y, { 0, 1, 0 });
    break;
  case MouseState::leftButtonPressed: // move
    pointer->defaultCamera.Move({ 0, delta.x / 50, -delta.y / 50 });  // z is up
    break;
  case MouseState::released:
  default: 
    break;
  }

  pointer->mousepos = glm::vec2((float)xpos, (float)ypos);
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

void lpe::Window::AddRenderObject(RenderObject* obj)
{
  if (!window)
    throw std::runtime_error("Cannot add model if the window wasn't created successfully. Call Create(...) before AddRenderObject(...)!");

  modelsRenderer.AddObject(obj);
  uniformBuffer.Update(defaultCamera, modelsRenderer, commands);
  commands.ResetCommandBuffers();
  commands.CreateCommandBuffers(swapChain.GetFramebuffers(), swapChain.GetExtent(), renderPass, graphicsPipeline, modelsRenderer, uniformBuffer);
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

  uniformBuffer.Update(defaultCamera, modelsRenderer, commands);

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