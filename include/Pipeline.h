#ifndef PIPELINE_H
#define PIPELINE_H
#include "stdafx.h"

#include <string>
#include "BufferMemory.h"

BEGIN_LPE

class Pipeline
{
public:

  struct CreateInfo
  {
    struct ShaderInfo
    {
      std::string fileName;
      std::vector<char> compiledCode;
      vk::ShaderStageFlagBits type;
      std::string entryPoint;

      ShaderInfo(const std::string& fileName,
                 const std::vector<char>& compiledCode,
                 vk::ShaderStageFlagBits type,
                 const std::string& entryPoint);
    };

    vk::PipelineBindPoint type;
    uint32_t prio;
    bool allowTransperency;

    std::vector<ShaderInfo> shaders;

    vk::RenderPass renderPass;
    vk::Extent2D swapChainExtent;
    std::vector<vk::DescriptorBufferInfo> descriptors;

    std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
    std::vector<vk::PushConstantRange> pushConstantRanges;
    std::vector<vk::WriteDescriptorSet> descriptorSets;
  };

private:
  vk::PhysicalDevice physicalDevice;
  std::unique_ptr<vk::Device> device;

  vk::PipelineCache cache;

  vk::DescriptorSetLayout descriptorSetLayout;
  vk::PipelineLayout pipelineLayout;
  vk::Pipeline pipeline;
  vk::DescriptorPool descriptorPool;
  vk::DescriptorSet descriptorSet;

  bool transparent;
  uint32_t priority;
  vk::PipelineBindPoint type;

  vk::ShaderModule CreateShaderModule(const std::vector<char>& code);

  void CreateDescriptorPool();
  void CreateDescriptorSetLayout();
  void CreatePipeline(vk::Extent2D swapChainExtent, vk::RenderPass renderPass, const CreateInfo& info);
  
public:
  Pipeline() = default;
  Pipeline(const Pipeline& other);
  Pipeline(Pipeline&& other) noexcept;
  Pipeline& operator=(const Pipeline& other);
  Pipeline& operator=(Pipeline&& other) noexcept;

  Pipeline(vk::PhysicalDevice physicalDevice, 
           vk::Device* device,
           vk::PipelineCache cache,
           const lpe::BufferMemory& memory,
           CreateInfo createInfo);

  ~Pipeline();

  void UpdateDescriptorSets(std::vector<vk::DescriptorBufferInfo> descriptors);

  vk::Pipeline GetPipeline() const;
  vk::PipelineLayout GetPipelineLayout() const;
  vk::DescriptorSet GetDescriptorSet() const;
  vk::DescriptorSet* GetDescriptorSetRef();

  bool AllowsTransparency() const;
  uint32_t GetPrio() const;
  vk::PipelineBindPoint GetType() const;
};

END_LPE

#endif
