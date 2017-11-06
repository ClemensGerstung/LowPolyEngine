#ifndef VERTEX_H
#define VERTEX_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include <array>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include "stdafx.h"

BEGIN_LPE
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normals;
    uint32_t instanceIndex;
    //glm::vec2 texCoord;

    Vertex() = default;
    Vertex(std::initializer_list<glm::vec3> list);

    static std::vector<vk::VertexInputBindingDescription> GetBindingDescription();

    static std::array<vk::VertexInputAttributeDescription, 4> GetAttributeDescriptions();

    bool operator==(const Vertex& other) const;
  };

END_LPE

namespace std
{
  template <>
  struct hash<lpe::Vertex>
  {
    size_t operator()(lpe::Vertex const& vertex) const
    {
      auto hash1 = hash<glm::vec3>()(vertex.position);
      auto hash2 = hash<glm::vec3>()(vertex.color);
      auto hash3 = hash<glm::vec3>()(vertex.normals);

      return ((hash1 ^ (hash2 << 1)) >> 1) ^ (hash3 << 1);
    }
  };
}

#endif
