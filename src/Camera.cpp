#include "../include/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

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

void lpe::Camera::Move(glm::vec3 move)
{
  position += move;
  //lookAt += move;
}

void lpe::Camera::Rotate(float degree, glm::vec3 axis)
{
  position = glm::rotate(position, glm::radians(degree), axis);
}

void lpe::Camera::Rotate(float degree)
{
  position = glm::rotate(position, glm::radians(degree), lookAt);
}

glm::mat4 lpe::Camera::GetView() const
{
  return glm::lookAt(position, lookAt, { 0.0f, 0.0f, 1.0f });
}

glm::mat4 lpe::Camera::GetPerspective() const
{
  return glm::perspective(glm::radians(fov), static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), near, far);
}
