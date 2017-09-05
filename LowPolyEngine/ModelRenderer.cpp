#include "ModelRenderer.h"
#include "UniformBufferObject.h"

void lpe::ModelRenderer::Create(vk::PhysicalDevice physicalDevice, const vk::Device& device, lpe::Commands& commands, const vk::Queue graphicsQueue, const lpe::Model& model)
{
	vertexBuffer = { physicalDevice, device };
	indicesBuffer = { physicalDevice, device };

	vk::DeviceSize vertexBufferSize = sizeof(model.GetVertices()[0]) * model.GetVertices().size();
	vk::DeviceSize indicesBufferSize = sizeof(model.GetIndices()[0]) * model.GetIndices().size();
	vk::DeviceSize uniformBufferSize = sizeof(UniformBufferObject);

	vertexBuffer.Create(commands, graphicsQueue, model.GetVertices().data(), vertexBufferSize);
	indicesBuffer.Create(commands, graphicsQueue, model.GetIndices().data(), indicesBufferSize);
	uniformBuffer.CreateBuffer(uniformBufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffer.buffer, uniformBuffer.memory);
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
