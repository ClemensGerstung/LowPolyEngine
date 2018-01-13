#ifndef MODELSRENDERER_H
#define MODELSRENDERER_H
#include <vector>
#include "Vertex.h"
#include "Buffer.h"
#include "Model.h"
#include <set>
#include "Commands.h"
#include "RenderObject.h"

BEGIN_LPE

using ObjectRef = RenderObject*;

class ModelsRenderer
{
private:


	vk::PhysicalDevice physicalDevice;
	std::unique_ptr<vk::Device> device;
	std::unique_ptr<Commands> commands;
	std::vector<ObjectRef> objects;

	std::vector<lpe::Vertex> vertices;
	std::vector<uint32_t> indices;

	Buffer vertexBuffer;
	Buffer indexBuffer;
	Buffer indirectBuffer;

	void Copy(const ModelsRenderer& other);
	void Move(ModelsRenderer& other);

public:
	ModelsRenderer() = default;
	ModelsRenderer(const ModelsRenderer& other);
	ModelsRenderer(ModelsRenderer&& other) noexcept;
	ModelsRenderer& operator=(const ModelsRenderer& other);
	ModelsRenderer& operator=(ModelsRenderer&& other) noexcept;

	ModelsRenderer(vk::PhysicalDevice physicalDevice, vk::Device* device, Commands* commands);

	~ModelsRenderer();

  void AddObject(ObjectRef obj);

	void UpdateBuffer();

  uint32_t GetOffet(uint32_t prio) const;
	uint32_t GetCount(uint32_t prio) const;
  uint32_t GetTotalCount() const;
	std::vector<lpe::Vertex> GetVertices() const;
	std::vector<uint32_t> GetIndices() const;
	vk::Buffer GetVertexBuffer();
	vk::Buffer GetIndexBuffer();

	vk::Buffer GetIndirectBuffer();

	bool Empty() const;
	uint32_t EntriesCount() const;

	std::vector<vk::DrawIndexedIndirectCommand> GetDrawIndexedIndirectCommands();

  std::vector<InstanceData> GetInstanceData() const;
};

END_LPE

#endif