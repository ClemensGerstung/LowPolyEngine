#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H
#include "stdafx.h"
#include <glm/mat4x4.hpp>
#include "Buffer.h"
#include "Model.h"
#include "Camera.h"
#include "UniformBufferObject.h"

BEGIN_LPE

class UniformBuffer
{
private:
  vk::PhysicalDevice physicalDevice;
  std::unique_ptr<vk::Device> device;

  UniformBufferObject ubo;
  size_t dynamicAlignment;
  size_t lastAllocSize = 0;

  struct UboDataDynamic {
    glm::mat4 *model = nullptr;
  } uboDataDynamic;

  Buffer viewBuffer;
  Buffer dynamicBuffer;

public:
  UniformBuffer() = default;
  UniformBuffer(const UniformBuffer& other);
  UniformBuffer(UniformBuffer&& other);
  UniformBuffer& operator=(const UniformBuffer& other);
  UniformBuffer& operator=(UniformBuffer&& other);

  UniformBuffer(vk::PhysicalDevice physicalDevice, vk::Device* device, const std::vector<Model>& models, const Camera& camera);

  ~UniformBuffer();

  void Update(const Camera& camera, const std::vector<Model>& models, bool force = false);

  std::vector<vk::DescriptorBufferInfo*> GetDescriptors();


  size_t GetDynamicAlignment() const;
};

END_LPE

#endif