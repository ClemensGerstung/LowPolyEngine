#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H
#include "stdafx.h"
#include <glm/mat4x4.hpp>
#include "Buffer.h"
#include "Model.h"
#include "Camera.h"
#include "UniformBufferObject.h"

BEGIN_LPE
  class ModelsRenderer;

  class UniformBuffer
  {
  private:
    vk::PhysicalDevice physicalDevice;
    std::unique_ptr<vk::Device> device;

    UniformBufferObject ubo;

    Buffer viewBuffer;
    Buffer instanceBuffer;

  public:
    UniformBuffer() = default;
    UniformBuffer(const UniformBuffer& other);
    UniformBuffer(UniformBuffer&& other);
    UniformBuffer& operator=(const UniformBuffer& other);
    UniformBuffer& operator=(UniformBuffer&& other);

    UniformBuffer(vk::PhysicalDevice physicalDevice,
                  vk::Device* device,
                  ModelsRenderer& modelsRenderer,
                  const Camera& camera,
                  const Commands& commands);

    ~UniformBuffer();

    void Update(const Camera& camera,
                ModelsRenderer& renderer,
                const Commands& commands);

    std::vector<vk::DescriptorBufferInfo> GetDescriptors();

    void SetLightPosition(glm::vec3 light);

    vk::Buffer GetInstanceBuffer();
  };

END_LPE

#endif
