#ifndef COMMANDS_H
#define COMMANDS_H
#include "stdafx.h"
#include "Buffer.h"
#include "ImageView.h"
#include "Pipeline.h"

BEGIN_LPE

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
  Commands(Commands&& other);
  Commands& operator=(const Commands& other);
  Commands& operator=(Commands&& other);

  Commands(vk::PhysicalDevice physicalDevice, vk::Device* device, vk::Queue* graphicsQueue, uint32_t graphicsFamilyIndex);

  ~Commands();

  void DeleteCommandBuffers();
  void CreateCommandBuffers(const std::vector<vk::Framebuffer>& framebuffers, vk::Extent2D extent, size_t dynamicAlignment, lpe::Pipeline* pipeline, ModelsRenderer* renderer);

  vk::CommandBuffer BeginSingleTimeCommands() const;
  void EndSingleTimeCommands(vk::CommandBuffer commandBuffer) const;

  lpe::Buffer CreateBuffer(void* data, vk::DeviceSize size) const;
  lpe::Buffer CreateBuffer(vk::DeviceSize size) const;
  lpe::ImageView CreateDepthImage(vk::Extent2D extent, vk::Format depthFormat) const;
};

END_LPE

#endif
