#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "stdafx.h"

BEGIN_LPE

class RenderPass
{
private:
  vk::RenderPass renderPass;
  std::unique_ptr<vk::Device> device;
  
  void CreateRenderPass(vk::Format swapChainImageFormat, vk::Format depthFormat, bool useDepth);

public:
  RenderPass() = default;
  RenderPass(const RenderPass& other);
  RenderPass(RenderPass&& other);
  RenderPass& operator=(const RenderPass& other);
  RenderPass& operator=(RenderPass&& other);

  RenderPass(std::unique_ptr<vk::Device> device, vk::Format swapChainImageFormat, vk::Format depthFormat);
  RenderPass(std::unique_ptr<vk::Device> device, vk::Format swapChainImageFormat);

  ~RenderPass();

  vk::RenderPass GetRenderPass() const;

  operator vk::RenderPass() const;
};

END_LPE

#endif
