#include "../include/UniformBuffer.h"
#include "../include/ModelsRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

lpe::UniformBuffer::UniformBuffer(const UniformBuffer& other)
{
  this->device.reset(other.device.get());
  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->viewBuffer = other.viewBuffer;
}

lpe::UniformBuffer::UniformBuffer(UniformBuffer&& other)
{
  this->device.reset(other.device.get());
  other.device.release();

  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->viewBuffer = std::move(other.viewBuffer);
}

lpe::UniformBuffer& lpe::UniformBuffer::operator=(const UniformBuffer& other)
{
  this->device.reset(other.device.get());
  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->viewBuffer = other.viewBuffer;

  return *this;
}

lpe::UniformBuffer& lpe::UniformBuffer::operator=(UniformBuffer&& other)
{
  this->device.reset(other.device.get());
  other.device.release();

  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->viewBuffer = std::move(other.viewBuffer);

  return *this;
}

lpe::UniformBuffer::UniformBuffer(vk::PhysicalDevice physicalDevice,
                                  vk::Device* device,
                                  ModelsRenderer& modelsRenderer,
                                  const Camera& camera)
  : physicalDevice(physicalDevice)
{
  this->device.reset(device);

  viewBuffer = {physicalDevice, device, sizeof(ubo)};
	
  
  Update(camera, modelsRenderer);
}

lpe::UniformBuffer::~UniformBuffer()
{
  if(device)
  {
    device.release();
  }
}

void lpe::UniformBuffer::Update(const Camera& camera, ModelsRenderer& renderer)
{
  ubo.view = camera.GetView();
  ubo.projection = camera.GetPerspective();
  ubo.projection[1][1] *= -1;

  viewBuffer.CopyToBufferMemory(&ubo, sizeof(ubo));

	std::vector<glm::mat4x4> instanceData = {};
	auto models = renderer.GetModels();

	for (const auto& model : models)
	{
		glm::mat4x4 data = glm::translate(glm::mat4(1.0f), model.GetPosition());
		data = data * model.GetModelMatrix();

		instanceData.push_back(data);
	}

	if (instanceData.size() > 0 && instanceData.size() * sizeof(glm::mat4x4) != instanceBuffer.GetSize())
	{
		vk::DeviceSize size = instanceData.size() * sizeof(glm::mat4x4);

		// totally dump and inefficient
		instanceBuffer = { physicalDevice, device.get(), size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eStorageBuffer };
	}

	if (instanceData.size() > 0)
	{
		instanceBuffer.CopyToBufferMemory(instanceData.data());
	}
}

std::vector<vk::DescriptorBufferInfo> lpe::UniformBuffer::GetDescriptors()
{
  return { viewBuffer.GetDescriptor() };
}

void lpe::UniformBuffer::SetLightPosition(glm::vec3 light)
{
  ubo.lightPos = light;
}

vk::Buffer lpe::UniformBuffer::GetInstanceBuffer()
{
	return instanceBuffer.GetBuffer();
}