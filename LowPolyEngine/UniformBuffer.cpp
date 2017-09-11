#include "UniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>

lpe::UniformBuffer::UniformBuffer(const UniformBuffer& other)
{

}

lpe::UniformBuffer::UniformBuffer(UniformBuffer&& other)
{

}

lpe::UniformBuffer& lpe::UniformBuffer::operator=(const UniformBuffer& other)
{
  return *this;
}

lpe::UniformBuffer& lpe::UniformBuffer::operator=(UniformBuffer&& other)
{
  return *this;
}

lpe::UniformBuffer::UniformBuffer(vk::PhysicalDevice physicalDevice,
                                  vk::Device* device,
                                  const std::vector<Model>& models,
                                  const Camera& camera)
  : physicalDevice(physicalDevice)
{
  this->device.reset(device);

  viewBuffer = {physicalDevice, device, sizeof(ubo)};
  
  ubo.view = camera.view;
  ubo.projection = camera.perspective;

  Update(camera, models);
}

void lpe::UniformBuffer::Update(const Camera& camera, const std::vector<Model>& models, bool force)
{
  ubo.view = camera.view;
  ubo.projection = camera.perspective;

  viewBuffer.CopyToBufferMemory(&ubo, sizeof(ubo));

  // Calculate required alignment depending on device limits
  // ¯\(°_o)/¯  https://github.com/SaschaWillems/Vulkan/blob/master/dynamicuniformbuffer/dynamicuniformbuffer.cpp#L414
  size_t uboAlignment = physicalDevice.getProperties().limits.minUniformBufferOffsetAlignment;
  size_t dynamicAlignment = (sizeof(glm::mat4) / uboAlignment) * uboAlignment + ((sizeof(glm::mat4) % uboAlignment) > 0 ? uboAlignment : 0);
  size_t bufferSize = models.size() * dynamicAlignment;

  uboDataDynamic.model = (glm::mat4*)helper::alignedAlloc(bufferSize, dynamicAlignment);
  assert(uboDataDynamic.model);

  // creating the buffer in every frame might be totally inefficient but otherwise it won't be totally dynamic
  dynamicBuffer = { physicalDevice, device.get(), bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible };

  for (uint64_t i = 0; i < models.size(); i++)
  {
    Model m = models[i];
    glm::mat4* modelMat = (glm::mat4*)(((uint64_t)uboDataDynamic.model + (i * dynamicAlignment)));

    *modelMat = glm::translate(glm::mat4(), m.GetPosition());
    *modelMat = m.GetModelMatrix();
  }

  dynamicBuffer.CopyToBufferMemory(uboDataDynamic.model);

  vk::MappedMemoryRange mappedMemoryRange = {*dynamicBuffer.GetMemory(), 0, dynamicBuffer.GetSize()};
  device->flushMappedMemoryRanges(1, &mappedMemoryRange);
}
