#include "../include/Vertex.h"
#include "../include/Instance.h"

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
    {1, sizeof(InstanceData), vk::VertexInputRate::eInstance}
  };

  return bindings;
}

std::vector<vk::VertexInputAttributeDescription> lpe::Vertex::GetAttributeDescriptions()
{
  std::vector<vk::VertexInputAttributeDescription> descriptions(7);

  // Per-Vertex attributes
  descriptions[0] = {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)};
  descriptions[1] = {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normals)};
  descriptions[2] = {2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)};
  
  // Per-Instance attributes
  descriptions[0] = { 3, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(InstanceData, row1) };
  descriptions[0] = { 4, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(InstanceData, row2) };
  descriptions[0] = { 5, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(InstanceData, row3) };
  descriptions[0] = { 6, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(InstanceData, row4) };

  return descriptions;
}

bool lpe::Vertex::operator==(const Vertex& other) const
{
  return position == other.position &&
         normals == other.normals &&
         color == other.color;
}
