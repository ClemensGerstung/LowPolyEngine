#ifndef IMGUI_H
#define IMGUI_H

#include "lpe.h"
#include "RenderPass.h"

BEGIN_LPE

class ImGUI
{
private:
  std::unique_ptr<vk::Device> device;

  int32_t vertexCount = 0;
  int32_t indexCount = 0;

  VkSampler sampler;
  VkDeviceMemory fontMemory = nullptr;
  VkImage fontImage = nullptr;
  VkImageView fontView = nullptr;
  VkDescriptorSet descriptorSet;

public:
  struct PushConstBlock {
    glm::vec2 scale;
    glm::vec2 translate;
  } pushConstBlock;

  ImGUI() = default;
  ImGUI(const ImGUI& other);
  ImGUI(ImGUI&& other);
  ImGUI& operator=(const ImGUI& other);
  ImGUI& operator=(ImGUI&& other);

  ImGUI(vk::Device* device, vk::PipelineCache pipelineCache, lpe::RenderPass& renderPass, vk::Extent2D extent);

  ~ImGUI();

  void RecordCommand(vk::CommandBuffer cmd);
  void UpdateBuffers();
  void NewFrame();
};

END_LPE

#endif