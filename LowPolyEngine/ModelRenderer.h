#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include "Buffer.h"
#include "Model.h"

namespace lpe
{
	class ModelRenderer
	{
		lpe::Buffer vertexBuffer;
		lpe::Buffer indicesBuffer;
		lpe::Buffer uniformBuffer;

	public:
		ModelRenderer() = default;
		~ModelRenderer() = default;

		void Create(vk::PhysicalDevice physicalDevice, const vk::Device& device, lpe::Commands& commands, const vk::Queue graphicsQueue, const lpe::Model& model);

		void UpdateUniformBuffer(const vk::Device& device, vk::Extent2D extent);

		lpe::Buffer GetVertexBuffer() const;
		lpe::Buffer GetIndicesBuffer() const;
		lpe::Buffer GetUniformBuffer() const;

		vk::Buffer* GetVertexBufferRef();
		vk::Buffer* GetIndicesBufferRef();
		vk::Buffer* GetUniformBufferRef();
	};
}

#endif