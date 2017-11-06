#ifndef DEVICE_H
#define DEVICE_H

#include "stdafx.h"
#include "QueueFamilyIndices.h"
#include "SwapChain.h"
#include "Commands.h"
#include "Pipeline.h"
#include "ModelsRenderer.h"
#include "RenderPass.h"

BEGIN_LPE

class Device
{
private:
  std::unique_ptr<vk::Instance> instance;
  vk::PhysicalDevice physicalDevice;
  vk::Device device;
  vk::SurfaceKHR surface;
  vk::Queue presentQueue;
  vk::Queue graphicsQueue;
  vk::PipelineCache pipelineCache;

  vk::Semaphore imageAvailableSemaphore;
  vk::Semaphore renderAvailableSemaphore;

  QueueFamilyIndices indices;
  vk::PipelineStageFlags waitFlags[1] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

public:
  Device() = default;
  Device(const Device&);
  Device(Device&&);
  Device & operator =(const Device&);
  Device & operator =(Device&&);

  Device(vk::Instance* instance, vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR& surface);
  ~Device();

  vk::Format FindDepthFormat() const;

  SwapChain CreateSwapChain(uint32_t width, uint32_t height);
  Commands CreateCommands();
  UniformBuffer CreateUniformBuffer(ModelsRenderer& modelsRenderer, const Camera& camera, const Commands& commands);
  Pipeline CreatePipeline(const SwapChain& swapChain, RenderPass& renderPass, UniformBuffer* ubo);
  ModelsRenderer CreateModelsRenderer(Commands* commands);
  RenderPass CreateRenderPass(vk::Format swapChainImageFormat);

  vk::SubmitInfo PrepareFrame(const SwapChain& swapChain, uint32_t* imageIndex);
  void SubmitQueue(uint32_t submitCount, const vk::SubmitInfo* infos);
  void SubmitFrame(const std::vector<vk::SwapchainKHR>& swapChains, uint32_t* imageIndex);

  explicit operator bool() const;
  bool operator!() const;
};

END_LPE

#endif
