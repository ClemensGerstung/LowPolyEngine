#ifndef PIPELINE_H
#define PIPELINE_H
#include "stdafx.h"
#include "UniformBuffer.h"

BEGIN_LPE

class Pipeline
{
private:
  vk::PhysicalDevice physicalDevice;
  std::unique_ptr<vk::Device> device;
  std::unique_ptr<lpe::UniformBuffer> ubo;

  vk::PipelineCache cache;

  vk::DescriptorSetLayout descriptorSetLayout;
  vk::PipelineLayout pipelineLayout;
  vk::Pipeline pipeline;
  vk::DescriptorPool descriptorPool;
  vk::DescriptorSet descriptorSet;

  vk::ShaderModule CreateShaderModule(const std::vector<char>& code);

  void CreateDescriptorPool();
  void CreateDescriptorSetLayout();
  void CreatePipeline(vk::Extent2D swapChainExtent, vk::RenderPass renderPass);
  
  void Copy(const Pipeline& other);
  void Move(Pipeline& other);

public:
  Pipeline() = default;
  Pipeline(const Pipeline& other);
  Pipeline(Pipeline&& other);
  Pipeline& operator=(const Pipeline& other);
  Pipeline& operator=(Pipeline&& other);

  // TODO: enhance to create compute pipeline or pipelines with other layout (e.g. wireframe only)
  Pipeline(vk::PhysicalDevice physicalDevice, 
           vk::Device* device,
           vk::PipelineCache cache,
           vk::RenderPass renderPass,
           vk::Extent2D swapChainExtent, 
           lpe::UniformBuffer* uniformBuffer);

  ~Pipeline();

  void CreateDescriptorSet();

  vk::Pipeline* GetPipelineRef();
  vk::PipelineLayout GetPipelineLayout() const;
  vk::DescriptorSet GetDescriptorSet() const;
  vk::DescriptorSet* GetDescriptorSetRef();
};

END_LPE

#endif
