#ifndef MODEL_H
#define MODEL_H
#include <string>
#include "stdafx.h"
#include "Vertex.h"

BEGIN_LPE

struct InstanceData
{
  glm::vec4 row1;
  glm::vec4 row2;
  glm::vec4 row3;
  glm::vec4 row4;
};

class Model
{
private:
  std::vector<lpe::Vertex> vertices;
  std::vector<uint32_t> indices;

  glm::vec3 position;
  glm::mat4 matrix;

  void Copy(const Model& other);
  void Move(Model& other);

public:
  Model();
  Model(const Model& other);
  Model(Model&& other);
  Model& operator=(const Model& other);
  Model& operator=(Model&& other);
  ~Model() = default;

  void Load(std::string fileName);


  void SetVertices(const std::vector<lpe::Vertex>& vertices);
  void SetIndices(const std::vector<uint32_t>& indices);

  void SetPosition(glm::vec3 position);
  void Move(glm::vec3 offset);


  void SetTransform(const glm::mat4& matrix);
  void Transform(glm::mat4 transform);

  glm::vec3 GetPosition() const;
  glm::mat4 GetModelMatrix() const;
  std::vector<lpe::Vertex> GetVertices() const;
  std::vector<uint32_t> GetIndices(uint32_t offset = 0);

  InstanceData GetInstanceData();

  void SetInstanceIndex(uint32_t instanceIndex);

  bool operator==(const Model& model) const;
};

END_LPE

#endif