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

	bool operator==(const Entry& e);
  };

  std::unique_ptr<Commands> commands;
  std::vector<Entry> entries;

  std::vector<lpe::Vertex> vertices;
  std::vector<uint32_t> indices;

  lpe::Buffer vertexBuffer;
  lpe::Buffer indexBuffer;

  void Copy(const ModelsRenderer& other);
  void Move(ModelsRenderer& other);

public:
  ModelsRenderer() = default;
  ModelsRenderer(const ModelsRenderer& other);
  ModelsRenderer(ModelsRenderer&& other);
  ModelsRenderer& operator=(const ModelsRenderer& other);
  ModelsRenderer& operator=(ModelsRenderer&& other);

  ModelsRenderer(Commands* commands);

  ~ModelsRenderer();

  Model* AddObject(std::string path);
  void RemoveObject(Model* model);
  
  void UpdateBuffer();

  std::vector<Model> GetModels() const;
  uint32_t GetCount() const;
  std::vector<lpe::Vertex> GetVertices() const;
  std::vector<uint32_t> GetIndices() const;
  vk::Buffer* GetVertexBuffer();
  vk::Buffer* GetIndexBuffer();

  bool Empty() const;
  uint32_t EntriesCount() const;

  Model operator[](uint32_t index) const;
};

END_LPE

#endif