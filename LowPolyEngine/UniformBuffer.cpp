#include "UniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>

lpe::UniformBuffer::UniformBuffer(const UniformBuffer& other)
{
  this->device.reset(other.device.get());
  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->uboDataDynamic = other.uboDataDynamic;
  this->dynamicBuffer = other.dynamicBuffer;
  this->viewBuffer = other.viewBuffer;
}

lpe::UniformBuffer::UniformBuffer(UniformBuffer&& other)
{
  this->device.reset(other.device.get());
  other.device.release();

  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->uboDataDynamic = other.uboDataDynamic;
  this->dynamicBuffer = std::move(other.dynamicBuffer);
  this->viewBuffer = std::move(other.viewBuffer);
}

lpe::UniformBuffer& lpe::UniformBuffer::operator=(const UniformBuffer& other)
{
  this->device.reset(other.device.get());
  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->uboDataDynamic = other.uboDataDynamic;
  this->dynamicBuffer = other.dynamicBuffer;
  this->viewBuffer = other.viewBuffer;

  return *this;
}

lpe::UniformBuffer& lpe::UniformBuffer::operator=(UniformBuffer&& other)
{
  this->device.reset(other.device.get());
  other.device.release();

  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->uboDataDynamic = other.uboDataDynamic;
  this->dynamicBuffer = std::move(other.dynamicBuffer);
  this->viewBuffer = std::move(other.viewBuffer);

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
  
  Update(camera, models);
}

lpe::UniformBuffer::~UniformBuffer()
{
  if (uboDataDynamic.model) 
  {
    helper::AlignedFree(uboDataDynamic.model);
  }

  if(device)
  {
    device.release();
  }
}

void lpe::UniformBuffer::Update(const Camera& camera, const std::vector<Model>& models, bool force)
{
  if(models.empty()) 
    return;

  ubo.view = camera.GetView();
  ubo.projection = camera.GetPerspective();

  viewBuffer.CopyToBufferMemory(&ubo, sizeof(ubo));

  // Calculate required alignment depending on device limits
  // ¯\(°_o)/¯  https://github.com/SaschaWillems/Vulkan/blob/master/dynamicuniformbuffer/dynamicuniformbuffer.cpp#L414
  size_t uboAlignment = physicalDevice.getProperties().limits.minUniformBufferOffsetAlignment;
  size_t dynamicAlignment = (sizeof(glm::mat4) / uboAlignment) * uboAlignment + ((sizeof(glm::mat4) % uboAlignment) > 0 ? uboAlignment : 0);
  size_t bufferSize = models.size() * dynamicAlignment;

  uboDataDynamic.model = (glm::mat4*)helper::AlignedAlloc(bufferSize, dynamicAlignment);
  assert(uboDataDynamic.model);

  // creating the buffer in every frame might be totally inefficient but otherwise it won't be totally dynamic
  dynamicBuffer = { physicalDevice, device.get(), bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible };

  for (uint64_t i = 0; i < models.size(); i++)
  {
    Model m = models[i];
    glm::mat4* modelMat = (glm::mat4*)(((uint64_t)uboDataDynamic.model + (i * dynamicAlignment)));

    *modelMat = glm::translate(glm::mat4(), m.GetPosition());
    *modelMat = *modelMat * m.GetModelMatrix(); // multiplying matrices should work fine...
  }

  dynamicBuffer.CopyToBufferMemory(uboDataDynamic.model);

  vk::MappedMemoryRange mappedMemoryRange = {*dynamicBuffer.GetMemory(), 0, dynamicBuffer.GetSize()};
  device->flushMappedMemoryRanges(1, &mappedMemoryRange);
}

std::vector<vk::DescriptorBufferInfo*> lpe::UniformBuffer::GetDescriptors()
{
  return { viewBuffer.GetDescriptor(), dynamicBuffer.GetDescriptor() };
}
