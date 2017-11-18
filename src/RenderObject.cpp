#include "../include/RenderObject.h"
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>

lpe::RenderInstance::RenderInstance()
{
  values.push({ });
  ResetTransform();
}

lpe::RenderInstance::RenderInstance(RenderInstance&& other)
{
  values = std::move(other.values);
}

lpe::RenderInstance::RenderInstance(const RenderInstance& other)
{
  values = { other.values };
}

lpe::RenderInstance& lpe::RenderInstance::operator=(const RenderInstance& other)
{
  values = { other.values };

  return *this;
}

lpe::RenderInstance& lpe::RenderInstance::operator=(RenderInstance&& other)
{
  values = std::move(other.values);

  return *this;
}

lpe::InstanceData lpe::RenderInstance::ToInstanceData()
{
  auto position = values.top().position;
  auto transform = values.top().matrix;

  InstanceData instanceData;

  glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
  matrix = matrix * transform;

  instanceData.row1 = matrix[0];
  instanceData.row2 = matrix[1];
  instanceData.row3 = matrix[2];
  instanceData.row4 = matrix[3];

  return instanceData;
}

void lpe::RenderInstance::ResetTransform()
{
  values.top().matrix = glm::mat4(1.0f);
  values.top().position = glm::vec3(0);
}

void lpe::RenderInstance::SetTransform(glm::mat4 transform)
{
  values.top().matrix = transform;
}

glm::mat4 lpe::RenderInstance::Transform(glm::mat4 transform)
{
  auto matrix = values.top().matrix;
  matrix = matrix * transform;
  values.top().matrix = matrix;

  return matrix;
}

void lpe::RenderInstance::SetPosition(glm::vec3 pos)
{
  values.top().position = pos;
}

void lpe::RenderInstance::Move(glm::vec3 delta)
{
  values.top().position += delta;
}

void lpe::RenderInstance::Push()
{
  values.push({});
  ResetTransform();
}

void lpe::RenderInstance::Pop()
{
  if(values.size() > 1)
  {
    values.pop();
  }
}

glm::vec3 lpe::RenderInstance::GetPosition() const
{
  return values.top().position;
}

glm::mat4 lpe::RenderInstance::GetTransform() const
{
  return values.top().matrix;
}

void lpe::RenderObject::Load(std::string fileName)
{
  // simply load ply file
  // ignoring header!

  std::ifstream file(fileName);

  if (!file)
  {
    std::runtime_error("Failed to open model " + fileName);
  }

  std::string line;
  bool header = true;
  uint32_t countVertices = ~0u;
  uint32_t countFaces = ~0u;

  while (std::getline(file, line))
  {
    if (header)
    {
      int32_t find = (int32_t)line.find("element vertex");
      if (find >= 0)
      {
        countVertices = std::stoi(line.substr(line.find_last_of(" ") + 1));
        continue;
      }

      find = (int32_t)line.find("element face");
      if (find >= 0)
      {
        countFaces = std::stoi(line.substr(line.find_last_of(" ") + 1));
        continue;
      }

      if (line == "end_header")
      {
        header = false;
        break;
      }
    }
  }

  if (header || countVertices == -1 || countFaces == -1)
  {
    throw std::runtime_error("file doesn't have the right format");
  }

  vertices.resize(countVertices);
  for (uint32_t i = 0; i < countVertices; i++)
  {
    std::getline(file, line);

    std::istringstream linepart(line);
    std::string part;
    uint16_t index = 0;
    Vertex v;

    while (std::getline(linepart, part, ' '))
    {
      if (index <= 2)
      {
        v.position[index] = std::stof(part);
      }
      else if (index <= 5)
      {
        v.normals[index % 3] = std::stof(part);
      }
      else if (index <= 8)
      {
        v.color[index % 3] = std::stof(part) / 255.f;
      }
      else
      {
        break;
      }
      index++;
    }

    vertices[i] = v;
  }

  indices.resize(countFaces * 3);
  for (uint32_t i = 0; i < countFaces; i++)
  {
    std::getline(file, line);

    std::istringstream linepart(line);
    std::string part;
    uint16_t index = 0;

    while (std::getline(linepart, part, ' '))
    {
      if (index == 0 && std::stoi(part) != 3)
      {
        throw std::runtime_error("faces are not triangulated!");
      }

      if (index > 0)
      {
        indices[(i * 3 + index - 1)] = std::stoi(part);
      }

      index++;
    }
  }

  file.close();
}

lpe::RenderObject::RenderObject(const RenderObject& other)
{
  prio = other.prio;
  vertexOffset = other.vertexOffset;
  indexOffset = other.indexOffset;

  instances = { other.instances };
  indices = { other.indices };
  vertices = { other.vertices };
}

lpe::RenderObject::RenderObject(RenderObject&& other) noexcept
{
  prio = other.prio;
  vertexOffset = other.vertexOffset;
  indexOffset = other.indexOffset;

  instances = std::move(other.instances);
  indices = std::move(other.indices);
  vertices = std::move(other.vertices);
}

lpe::RenderObject& lpe::RenderObject::operator=(const RenderObject& other)
{
  prio = other.prio;
  vertexOffset = other.vertexOffset;
  indexOffset = other.indexOffset;

  instances = { other.instances };
  indices = { other.indices };
  vertices = { other.vertices };

  return *this;
}

lpe::RenderObject& lpe::RenderObject::operator=(RenderObject&& other) noexcept
{
  prio = other.prio;
  vertexOffset = other.vertexOffset;
  indexOffset = other.indexOffset;

  instances = std::move(other.instances);
  indices = std::move(other.indices);
  vertices = std::move(other.vertices);

  return *this;
}

lpe::RenderObject::RenderObject(std::string path, uint32_t prio)
  : prio(prio)
{
  Load(path);
}

void lpe::RenderObject::SetOffsets(uint32_t indexOffset, int32_t vertexOffset)
{
  this->indexOffset = indexOffset;
  this->vertexOffset = vertexOffset;
}

lpe::InstanceRef lpe::RenderObject::GetInstance(uint32_t id)
{
  auto result = instances.find(id);

  if (result == instances.end())
  {
    instances.insert(std::make_pair(id, RenderInstance{}));
  }

  auto returnRef = InstanceRef(&instances.at(id));
  
  return returnRef;
}

void lpe::RenderObject::EreaseInstance(uint32_t id)
{
  instances.erase(id);
}

vk::DrawIndexedIndirectCommand lpe::RenderObject::GetIndirectCommand(uint32_t existingInstances) const
{
  vk::DrawIndexedIndirectCommand cmd = { (uint32_t)indices.size(), (uint32_t)instances.size(), indexOffset, vertexOffset, existingInstances };

  return cmd;
}

std::vector<lpe::InstanceData> lpe::RenderObject::GetInstanceData()
{
  std::vector<lpe::InstanceData> data(instances.size());

  for (uint32_t i = 0; i < data.size(); ++i)
  {
    auto iterator = instances.begin();

    for (uint32_t j = 0; j < i; ++j)
    {
      ++iterator;
    }

    data[i] = (*iterator).second.ToInstanceData();
  }

  return data;
}

uint32_t lpe::RenderObject::GetInstanceCount() const
{
  return  (uint32_t)instances.size();
}

std::vector<lpe::Vertex>::iterator lpe::RenderObject::GetVertexBegin()
{
  return std::begin(vertices);
}

std::vector<lpe::Vertex>::iterator lpe::RenderObject::GetVertexEnd()
{
  return std::end(vertices);
}

std::vector<uint32_t>::iterator lpe::RenderObject::GetIndexBegin()
{
  return std::begin(indices);
}

std::vector<uint32_t>::iterator lpe::RenderObject::GetIndexEnd()
{
  return std::end(indices);
}
