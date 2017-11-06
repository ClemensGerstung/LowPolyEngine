#include "../include/Vertex.h"

lpe::Vertex::Vertex(std::initializer_list<glm::vec3> list)
{
  if (list.size() == 3)
  {
    position = *list.begin();
    color = *(list.begin() + 1);
    normals = *(list.begin() + 2);
  }

  if (list.size() == 2)
  {
    position = *list.begin();
    color = *(list.begin() + 1);
  }
}

std::vector<vk::VertexInputBindingDescription> lpe::Vertex::GetBindingDescription()
{
  std::vector<vk::VertexInputBindingDescription> bindings =
  {
    {0, sizeof(Vertex), vk::VertexInputRate::eVertex},
    {1, sizeof(glm::mat4x4), vk::VertexInputRate::eInstance}
  };

  return bindings;
}

std::array<vk::VertexInputAttributeDescription, 4> lpe::Vertex::GetAttributeDescriptions()
{
  std::array<vk::VertexInputAttributeDescription, 4> descriptions = {};
  descriptions[0] = {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)};
  descriptions[1] = {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normals)};
  descriptions[2] = {2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)};
  descriptions[3] = {3, 0, vk::Format::eR32Uint, offsetof(Vertex, instanceIndex)};

  return descriptions;
}

bool lpe::Vertex::operator==(const Vertex& other) const
{
  return position == other.position &&
         normals == other.normals &&
         color == other.color;
}
