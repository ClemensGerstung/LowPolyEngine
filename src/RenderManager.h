#pragma once

#include "ServiceBase.h"
#include "VkMemoryManagement.h"
#include "utils.h"
#include "RenderObject.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace rendering
  {
    class IRenderManager : public ServiceBase
    {
    public:
      ~IRenderManager() override = default;

      virtual void Draw() = 0;

      //virtual void AddRenderObject(std::weak_ptr<lpe::rendering::RenderObject> renderObject) = 0;
    };
  }
}
