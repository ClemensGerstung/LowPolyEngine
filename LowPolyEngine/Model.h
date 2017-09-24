#ifndef MODEL_H
#define MODEL_H
#include <string>
#include "stdafx.h"
#include "Vertex.h"

BEGIN_LPE

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
  Model() = default;
  Model(const Model& other);
  Model(Model&& other);
  Model& operator=(const Model& other);
  Model& operator=(Model&& other);
  ~Model() = default;

  void Load(std::string fileName);

  void SetPosition(glm::vec3 position);
  void Move(glm::vec3 offset);

  void Transform(glm::mat4 transform);

  glm::vec3 GetPosition() const;
  glm::mat4 GetModelMatrix() const;
  std::vector<lpe::Vertex> GetVertices() const;
  std::vector<uint32_t> GetIndices(uint32_t offset = 0);

  bool operator==(const Model& model);
};

END_LPE

#endif