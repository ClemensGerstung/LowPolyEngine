#include "ModelRenderer.h"
#include "UniformBufferObject.h"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

void lpe::ModelRenderer::Create(vk::PhysicalDevice physicalDevice, const vk::Device& device, lpe::Commands& commands, const vk::Queue graphicsQueue, const lpe::Model& model)
{
	Buffer vertexBuffer = { physicalDevice, device };
	Buffer indicesBuffer = { physicalDevice, device };
	Buffer uniformBuffer = { physicalDevice, device };

	vk::DeviceSize vertexBufferSize = sizeof(model.GetVertices()[0]) * model.GetVertices().size();
	vk::DeviceSize indicesBufferSize = sizeof(model.GetIndices()[0]) * model.GetIndices().size();
	vk::DeviceSize uniformBufferSize = sizeof(UniformBufferObject);

	vertexBuffer.Create(commands, graphicsQueue, model.GetVertices().data(), vertexBufferSize);
	indicesBuffer.Create(commands, graphicsQueue, model.GetIndices().data(), indicesBufferSize);
	uniformBuffer.CreateBuffer(uniformBufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffer.buffer, uniformBuffer.memory);

	this->vertexBuffer = std::move(vertexBuffer);
	this->indicesBuffer = std::move(indicesBuffer);
	this->uniformBuffer = std::move(uniformBuffer);
}

void lpe::ModelRenderer::UpdateUniformBuffer(const vk::Device& device, vk::Extent2D extent)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	device.mapMemory(uniformBuffer.memory, 0, sizeof(ubo), {}, &data);
	memcpy(data, &ubo, sizeof(ubo));
	device.unmapMemory(uniformBuffer.memory);
}

lpe::Buffer lpe::ModelRenderer::GetVertexBuffer() const
{
	return vertexBuffer;
}

lpe::Buffer lpe::ModelRenderer::GetIndicesBuffer() const
{
	return indicesBuffer;
}

lpe::Buffer lpe::ModelRenderer::GetUniformBuffer() const
{
	return uniformBuffer;
}

vk::Buffer& lpe::ModelRenderer::GetVertexBufferRef()
{
	return vertexBuffer.buffer;
}

vk::Buffer& lpe::ModelRenderer::GetIndicesBufferRef()
{
	return indicesBuffer.buffer;
}

vk::Buffer& lpe::ModelRenderer::GetUniformBufferRef()
{
	return uniformBuffer.buffer;
}
