#ifndef COMMANDS_H
#define COMMANDS_H
#include "stdafx.h"
#include "ImageView.h"
#include "Pipeline.h"

#include <map>
#include "Renderer.h"

BEGIN_LPE
  class RenderPass;
  class ModelsRenderer;

  class Commands
  {
  private:
    vk::PhysicalDevice physicalDevice;
    std::unique_ptr<vk::Device> device;
    std::unique_ptr<vk::Queue> graphicsQueue;
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;

  public:
    Commands() = default;
    Commands(const Commands& other);
    Commands(Commands&& other) noexcept;
    Commands& operator=(const Commands& other);
    Commands& operator=(Commands&& other) noexcept;

    Commands(vk::PhysicalDevice physicalDevice,
             vk::Device* device,
             vk::Queue* graphicsQueue,
             uint32_t graphicsFamilyIndex);

    ~Commands();

    void ResetCommandBuffers();
    void RecordCommandBuffers(const std::vector<vk::Framebuffer>& framebuffers,
                              vk::Extent2D extent,
                              RenderPass& renderPass,
                              const std::vector<lpe::Pipeline>& pipelines,
                              Renderer& renderer);

    vk::CommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(vk::CommandBuffer commandBuffer) const;

    lpe::ImageView CreateDepthImage(vk::Extent2D extent,
                                    vk::Format depthFormat) const;

    vk::CommandBuffer operator[](uint32_t index);
  };

END_LPE

#endif
