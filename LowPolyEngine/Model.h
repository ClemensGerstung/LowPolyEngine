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

public:
  Model() = default;
  Model(const Model& other);
  Model(Model&& other);
  Model& operator=(const Model& other);
  Model& operator=(Model&& other);

  void Load(std::string fileName);

  void SetPosition(glm::vec3 position);
  void Move(glm::vec3 offset);

  void Transform(glm::mat4 transform);

  glm::vec3 GetPosition();
  glm::mat4 GetModelMatrix();
  std::vector<lpe::Vertex> GetVertices();
  std::vector<uint32_t> GetIndices();
};

END_LPE

#endif