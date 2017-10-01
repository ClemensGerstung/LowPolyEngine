#ifndef UNIFOR_BUFFER_OBJECT_H
#define UNIFOR_BUFFER_OBJECT_H

#include <glm/glm.hpp>

BEGIN_LPE

struct UniformBufferObject
{
  glm::mat4 projection;
  glm::mat4 view;
};

END_LPE

#endif
