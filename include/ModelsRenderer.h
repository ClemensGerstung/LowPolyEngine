#ifndef MODELSRENDERER_H
#define MODELSRENDERER_H
#include <vector>
#include "Vertex.h"
#include "Buffer.h"
#include "Model.h"
#include <set>
#include "Commands.h"

BEGIN_LPE

class ModelsRenderer
{
private:
	struct Entry
	{
		lpe::Model model;
		uint32_t verticesStartIndex;
		uint32_t indicesStartIndex;
		uint32_t verticesLength;
		uint32_t indicesLength;

    uint32_t instanceCount;

		bool operator==(const Entry& e);
	};

	vk::PhysicalDevice physicalDevice;
	std::unique_ptr<vk::Device> device;
	std::unique_ptr<Commands> commands;
	std::vector<Entry> entries;
	std::vector<Model> models;

	std::vector<lpe::Vertex> vertices;
	std::vector<uint32_t> indices;

	lpe::Buffer vertexBuffer;
	lpe::Buffer indexBuffer;
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

	void AddObject(std::vector<lpe::Vertex> vertices, std::vector<uint32_t> indices);
  void AddObject(std::string path);
	void RemoveObject(Model* model);

	void UpdateBuffer();

	std::vector<Model> GetModels();
	uint32_t GetCount() const;
	std::vector<lpe::Vertex> GetVertices() const;
	std::vector<uint32_t> GetIndices() const;
	vk::Buffer GetVertexBuffer();
	vk::Buffer GetIndexBuffer();

	vk::Buffer GetIndirectBuffer();

	bool Empty() const;
	uint32_t EntriesCount() const;

	Model operator[](uint32_t index) const;

	std::vector<vk::DrawIndexedIndirectCommand> GetDrawIndexedIndirectCommands();

  std::vector<InstanceData> GetInstanceData() const;

  Model* GetModelRef(int index);
};

END_LPE

#endif