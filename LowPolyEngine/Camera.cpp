#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

void lpe::Camera::Copy(const Camera& other)
{
  this->swapChainExtent = other.swapChainExtent;
  this->far = other.far;
  this->fov = other.fov;
  this->near = other.near;
  this->lookAt = other.lookAt;
  this->position = other.position;
}

lpe::Camera::Camera(const Camera& other)
{
  Copy(other);
}

lpe::Camera::Camera(Camera&& other)
{
  Copy(other);
}

lpe::Camera& lpe::Camera::operator=(const Camera& other)
{
  Copy(other);
  return *this;
}

lpe::Camera& lpe::Camera::operator=(Camera&& other)
{
  Copy(other);
  return *this;
}

lpe::Camera::Camera(glm::vec3 position, glm::vec3 lookAt, vk::Extent2D swapChainExtent, float fov, float near, float far)
  : swapChainExtent(swapChainExtent),
    fov(fov),
    near(near),
    far(far),
    position(position),
    lookAt(lookAt)
{
}

glm::mat4 lpe::Camera::GetView() const
{
  return glm::lookAt(position, lookAt, { 0.0f, 0.0f, 1.0f });
}

glm::mat4 lpe::Camera::GetPerspective() const
{
  return glm::perspectiveFov(glm::radians(fov), (float)swapChainExtent.width, (float)swapChainExtent.height, near, far);
}
