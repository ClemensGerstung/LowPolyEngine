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
    std::shared_ptr<lpe::Model> model;
    uint32_t verticesStartIndex;
    uint32_t indicesStartIndex;
    uint32_t verticesLength;
    uint32_t indicesLength;

    bool operator<(const Entry& e) const;
    bool operator==(const Entry& e) const;
  };

  std::unique_ptr<Commands> commands;
  std::set<Entry> entries;

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

  void AddObject(Model* model);
  void RemoveObject(Model* model);
  
  void UpdateBuffer();

  std::vector<Model> GetModels() const;
};

END_LPE

#endif