#ifndef DEVICE_H
#define DEVICE_H

#include "stdafx.h"
#include "QueueFamilyIndices.h"
#include "SwapChain.h"
#include "Commands.h"
#include "Pipeline.h"
#include "RenderPass.h"

#include <map>
#include "BufferMemory.h"
#include "Renderer.h"

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
    Device(Device&&) noexcept;
    Device& operator =(const Device&);
    Device& operator =(Device&&) noexcept;

    Device(vk::Instance* instance,
           vk::PhysicalDevice physicalDevice,
           const vk::SurfaceKHR& surface);
    ~Device();

    vk::Format FindDepthFormat() const;

    SwapChain CreateSwapChain(uint32_t width,
                              uint32_t height);
    Commands CreateCommands();
    Renderer CreateRenderer(lpe::BufferMemory* bufferMemory,
                            uint32_t bufferId);
    std::vector<lpe::Pipeline> CreatePipelines(const SwapChain& swapChain,
                                               RenderPass& renderPass,
                                               BufferMemory* buffer,
                                               uint32_t bufferId);
    ModelsRenderer CreateModelsRenderer(Commands* commands);
    RenderPass CreateRenderPass(vk::Format swapChainImageFormat);

    vk::SubmitInfo PrepareFrame(const SwapChain& swapChain,
                                uint32_t* imageIndex);
    void SubmitQueue(uint32_t submitCount,
                     const vk::SubmitInfo* infos);
    void SubmitFrame(const std::vector<vk::SwapchainKHR>& swapChains,
                     uint32_t* imageIndex);

    explicit operator bool() const;
    bool operator!() const;

    lpe::BufferMemory CreateBuffer(uint32_t vertexCount,
                                   uint32_t indexCount,
                                   uint32_t instanceCount,
                                   uint32_t indirectCommandsCount);
  };

END_LPE

#endif
