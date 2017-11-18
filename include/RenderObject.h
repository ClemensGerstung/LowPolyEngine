#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "lpe.h"
#include "Model.h"
#include <stack>
#include <unordered_map>

BEGIN_LPE

struct InstanceValue
{
  glm::vec3 position;
  glm::mat4 matrix;
};

class RenderInstance
{
private:
  std::stack<InstanceValue> values;
public:
  RenderInstance();
  RenderInstance(const RenderInstance& other);
  RenderInstance(RenderInstance&& other);
  RenderInstance& operator=(const RenderInstance& other);
  RenderInstance& operator=(RenderInstance&& other);

  ~RenderInstance() = default;

  InstanceData ToInstanceData();

  void ResetTransform();
  void SetTransform(glm::mat4 transform);
  glm::mat4 Transform(glm::mat4 transform);

  void SetPosition(glm::vec3 pos);
  void Move(glm::vec3 delta);

  void Push();
  void Pop();

  glm::vec3 GetPosition() const;
  glm::mat4 GetTransform() const;
};

class Deleter
{
public:
  void operator()(RenderInstance* instance) const {}
};

using InstanceRef = std::unique_ptr<RenderInstance, Deleter>;

class RenderObject
{
private:
  uint32_t prio;
  int32_t vertexOffset;
  uint32_t indexOffset;

  std::unordered_map<uint32_t, RenderInstance> instances;
  std::vector<uint32_t> indices;
  std::vector<lpe::Vertex> vertices;

  void Load(std::string fileName);

public:
  RenderObject() = default;
  RenderObject(const RenderObject& other);
  RenderObject(RenderObject&& other) noexcept;
  RenderObject& operator=(const RenderObject& other);
  RenderObject& operator=(RenderObject&& other) noexcept;

  RenderObject(std::string path, uint32_t prio);

  void SetOffsets(uint32_t indexOffset, int32_t vertexOffset);

  InstanceRef GetInstance(uint32_t id = 0);
  void EreaseInstance(uint32_t id);

  vk::DrawIndexedIndirectCommand GetIndirectCommand(uint32_t existingInstances) const;
  std::vector<InstanceData> GetInstanceData();

  uint32_t GetInstanceCount() const;

  std::vector<Vertex>::iterator GetVertexBegin();
  std::vector<Vertex>::iterator GetVertexEnd();
  std::vector<uint32_t>::iterator GetIndexBegin();
  std::vector<uint32_t>::iterator GetIndexEnd();
};

END_LPE

#endif