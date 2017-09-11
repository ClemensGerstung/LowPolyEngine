#ifndef Camera_H
#define Camera_H
#include "stdafx.h"
#include <glm/mat4x4.hpp>

BEGIN_LPE

class Camera
{
  friend class UniformBuffer;

private:
  glm::mat4 view;
  glm::mat4 perspective;

  glm::vec3 position;
  glm::vec3 lookAt;
public:
  Camera() = default;
  Camera(const Camera& other);
  Camera(Camera&& other);
  Camera& operator=(const Camera& other);
  Camera& operator=(Camera&& other);

  ~Camera() = default;
};

END_LPE

#endif
