#ifndef PIPELINE_H
#define PIPELINE_H
#include "stdafx.h"

BEGIN_LPE

class Pipeline
{
private:
  vk::PhysicalDevice physicalDevice;
  std::unique_ptr<vk::Device> device;

  vk::RenderPass renderPass;
  vk::DescriptorSetLayout descriptorSetLayout;
  vk::PipelineLayout pipelineLayout;
  vk::Pipeline graphicsPipeline;
  vk::DescriptorPool descriptorPool;
  vk::DescriptorSet descriptorSet;

  void CreateRenderPass(vk::Format swapChainImageFormat);
  void CreateDescriptorPool();
  void CreateDescriptorSetLayout();

  vk::ShaderModule CreateShaderModule(const std::vector<char>& code);

public:
  Pipeline() = default;
  Pipeline(const Pipeline& other);
  Pipeline(Pipeline&& other);
  Pipeline& operator=(const Pipeline& other);
  Pipeline& operator=(Pipeline&& other);

  Pipeline(vk::PhysicalDevice physicalDevice, vk::Device* device, vk::Format swapChainImageFormat, vk::Extent2D swapChainExtent);

  ~Pipeline();

  vk::Format FindDepthFormat() const;
};

END_LPE

#endif
