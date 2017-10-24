#ifndef Camera_H
#define Camera_H
#include "stdafx.h"
#include <glm/mat4x4.hpp>

BEGIN_LPE

class Camera
{
private:
  vk::Extent2D swapChainExtent;
  float fov;
  float near;
  float far;

  glm::vec3 position;
  glm::vec3 lookAt;

  void Copy(const Camera& other);
public:
  Camera() = default;
  Camera(const Camera& other);
  Camera(Camera&& other);
  Camera& operator=(const Camera& other);
  Camera& operator=(Camera&& other);

  Camera(glm::vec3 position, glm::vec3 lookAt, vk::Extent2D swapChainExtent, float fov = 60, float near = 0.0, float far = 10);

  ~Camera() = default;

  void Move(glm::vec3 move);
  void Rotate(float degree, glm::vec3 axis);
  void Rotate(float degree);

  glm::mat4 GetView() const;
  glm::mat4 GetPerspective() const;


  glm::vec3 GetPosition() const
  {
    return position;
  }

  glm::vec3 GetLookAt() const
  {
    return lookAt;
  }

	void SetFoV(float fov)
	{
		this->fov = fov;
	}

	void ChangeFoV(float delta)
	{
		this->fov += delta;
	}

	float GetFoV()
	{
		return fov;
	}
};

END_LPE

#endif
