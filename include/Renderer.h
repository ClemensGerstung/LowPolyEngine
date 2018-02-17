#ifndef RENDERER_H
#define RENDERER_H

#include "lpe.h"
#include "UniformBufferObject.h"
#include "Vertex.h"
#include "RenderObject.h"
#include "BufferMemory.h"
#include "Camera.h"

BEGIN_LPE
  using ObjectRef = RenderObject*;

  class Renderer
  {
  private:
    vk::PhysicalDevice physicalDevice;
    vk::PhysicalDeviceFeatures features;
    std::unique_ptr<vk::Device, helper::Deleter<vk::Device>> device;

    std::unique_ptr<lpe::BufferMemory, helper::Deleter<lpe::BufferMemory>> buffer;
    uint32_t bufferId;

    UniformBufferObject ubo;
    std::map<uint32_t, std::vector<ObjectRef>> objects;

    std::vector<lpe::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<lpe::InstanceData> instances;
    std::vector<vk::DrawIndexedIndirectCommand> commands;
  public:
    Renderer() = default;
    Renderer(const Renderer& other);
    Renderer(Renderer&& other);
    Renderer& operator=(const Renderer& other);
    Renderer& operator=(Renderer&& other);

    Renderer(vk::Device* device,
             vk::PhysicalDevice physicalDevice,
             lpe::BufferMemory* bufferMemory,
             uint32_t bufferId);

    ~Renderer() = default;

    void AddObject(ObjectRef obj);
    //void RemoveObject(ObjectRef obj);

    void UpdateBuffer(const lpe::Camera& camera);
    void Record(uint32_t prio,
                vk::CommandBuffer cmd);

    uint32_t GetOffet(uint32_t prio) const;
    uint32_t GetCount(uint32_t prio) const;
    //uint32_t GetTotalCount() const;
    bool Empty() const;
    //uint32_t EntriesCount() const;

    //tmp
    void SetLightPosition(glm::vec3 pos);
  };

END_LPE

#endif
