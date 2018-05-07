#include "Window.h"

void lpe::window::Window::GLFW_KeyInputCallback(GLFWwindow* window,
                                                int key,
                                                int scancode,
                                                int action,
                                                int mods)
{
  auto* pointer = reinterpret_cast<lpe::window::Window*>(glfwGetWindowUserPointer(window));
  pointer->KeyPressed(*pointer,
                      {
                        static_cast<Key>(key),
                        static_cast<State>(action),
                        static_cast<int32_t>(scancode),
                        static_cast<KeyModifiers>(mods)
                      });
}

void lpe::window::Window::GLFW_MouseInputCallback(GLFWwindow* window,
                                                  int button,
                                                  int action,
                                                  int mods)
{
  auto* pointer = reinterpret_cast<lpe::window::Window*>(glfwGetWindowUserPointer(window));
  pointer->MousePressed(*pointer,
                        {
                          static_cast<MouseButton>(button),
                          static_cast<State>(action),
                          static_cast<KeyModifiers>(mods)
                        });
}

void lpe::window::Window::GLFW_MouseMoveCallback(GLFWwindow* window,
                                                 double xpos,
                                                 double ypos)
{
  auto* pointer = reinterpret_cast<lpe::window::Window*>(glfwGetWindowUserPointer(window));
  pointer->MouseMoved(*pointer,
                      {
                        pointer->x,
                        pointer->y,
                        xpos,
                        ypos
                      });
  pointer->x = xpos;
  pointer->y = ypos;
}


void lpe::window::Window::GLFW_WindowResizeCallback(GLFWwindow* window,
                                                    int width,
                                                    int height)
{
  auto* pointer = reinterpret_cast<lpe::window::Window*>(glfwGetWindowUserPointer(window));
  pointer->Resized(*pointer,
                   {
                     pointer->width,
                     pointer->height,
                     static_cast<uint32_t>(width),
                     static_cast<uint32_t>(height)
                   });
  pointer->width = width;
  pointer->height = height;
}

lpe::window::Window::Window(const std::string&& title,
                            uint32_t width,
                            uint32_t height,
                            bool resizeable)
  : width(width),
    height(height),
    x(0.0),
    y(0.0),
    title(title)
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API,
                 GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE,
                 resizeable ?
                   GLFW_TRUE :
                   GLFW_FALSE);
  window = glfwCreateWindow(width,
                            height,
                            title.c_str(),
                            nullptr,
                            nullptr);

  glfwSetWindowUserPointer(window,
                           this);

  glfwSetKeyCallback(window,
                     GLFW_KeyInputCallback);
  glfwSetMouseButtonCallback(window,
                             GLFW_MouseInputCallback);
  glfwSetCursorPosCallback(window,
                           GLFW_MouseMoveCallback);
  glfwSetWindowSizeCallback(window,
                            GLFW_WindowResizeCallback);

  glfwGetCursorPos(window,
                   &x,
                   &y);
}

lpe::window::Window::~Window()
{
  if(window)
  {
    glfwDestroyWindow(window);
    window = nullptr;
  }
}

void lpe::window::Window::Render()
{
  if (!window) throw std::runtime_error("Cannot render on a window if there is no window!");

  glfwPollEvents();
}

bool lpe::window::Window::IsOpen() const
{
  if (!window) throw std::runtime_error("Tried to get window state of not existing window");

  return !glfwWindowShouldClose(window);
}

VkSurfaceKHR lpe::window::Window::GetWindowSurface(VkInstance instance) const
{
  VkSurfaceKHR surface;

  if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create window surface!");
  }

  return surface;
}

std::vector<const char*> lpe::window::Window::GetRequiredVulkanExtensions()
{
  std::vector<const char*> extensions;

  unsigned int glfwExtensionCount = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  for (unsigned int i = 0; i < glfwExtensionCount; i++)
  {
    extensions.push_back(glfwExtensions[i]);
  }

  return extensions;
}
