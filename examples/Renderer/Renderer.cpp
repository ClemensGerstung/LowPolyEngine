#include <lpe/lpe.h>

#include <stb_image.h>

#define _MK_PTR(T, n) std::shared_ptr<T> n = std::make_shared<T>()

int main()
{
  lpe::Initialize();

  auto logger = lpe::ServiceLocator::LogManager.Get().lock();
  assert(logger);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

//  GLFWwindow *window = glfwCreateWindow(1920, 1080, "LPE - Example - Renderer", nullptr, nullptr);

  lpe::rendering::vulkan::VulkanManager renderer = {};
//  renderer.AddInstanceExtension("VK_KHR_device_group_creation");
//
//  uint32_t count;
//  auto extensions = glfwGetRequiredInstanceExtensions(&count);
//  for (int i = 0; i < count; ++i)
//  {
//    renderer.AddInstanceExtension(extensions[i]);
//    logger->Log(extensions[i]);
//  }
//
//  renderer.LinkGlfwWindow(window)
//          .Initialize();
//
//  lpe::ServiceLocator::RenderManager.Provide(&renderer);
//
//  auto resourceManager = lpe::ServiceLocator::ResourceManager.Get()
//                                                             .lock();
//
//  assert(resourceManager);
//
//  _MK_PTR(lpe::rendering::Texture, t);
//  t->SetImage(resourceManager->Load("textures/lpe.jpg"));
//  t->SetColor({0, 255, 0, 1});
//
//  lpe::rendering::VkTexture2D texture2D = {};
//  texture2D.SetFormat(vk::Format::eR8G8B8A8Unorm)
//           .SetLayers(1)
//           .SetLayout(vk::ImageLayout::eUndefined)
//           .SetMipLevels(1)
//           .SetSamples(vk::SampleCountFlagBits::e1)
//           .SetTiling(vk::ImageTiling::eOptimal)
//           .SetUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
//           .Create(renderer, t);

//  _MK_PTR(lpe::rendering::Material, m);
//  m->SetAlbedo(t);
//
//  _MK_PTR(lpe::rendering::RenderTarget, r);
//  r->SetMaterial(m);
//  r->SetMesh(resourceManager->Load("models/cube.ply"));
//  r->SetVertexShader(resourceManager->Load("shaders/base.vert.spv"));
//  r->SetFragmentShader(resourceManager->Load("shaders/base.frag.spv"));
//
//  _MK_PTR(lpe::rendering::RenderObject, o);
//  o->AddTarget(r);


  return 0;
}
