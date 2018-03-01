#include "../include/Renderer.h"

lpe::Renderer::Renderer(const Renderer& other)
{
  device.reset(other.device.get());
  buffer.reset(other.buffer.get());
  physicalDevice = other.physicalDevice;
  features = other.features;
  bufferId = other.bufferId;
  ubo = other.ubo;
  objects = { other.objects };
  vertices = { other.vertices };
  indices = { other.indices };
  instances = { other.instances };
}

lpe::Renderer::Renderer(Renderer&& other)
{
  device = std::move(other.device);
  buffer = std::move(other.buffer);
  physicalDevice = other.physicalDevice;
  features = other.features;
  bufferId = other.bufferId;
  ubo = other.ubo;
  objects = std::move(other.objects);
  vertices = std::move(other.vertices);
  indices = std::move(other.indices);
  instances = std::move(other.instances);
}

lpe::Renderer& lpe::Renderer::operator=(const Renderer& other)
{
  device.reset(other.device.get());
  buffer.reset(other.buffer.get());
  physicalDevice = other.physicalDevice;
  features = other.features;
  bufferId = other.bufferId;
  ubo = other.ubo;
  objects = { other.objects };
  vertices = { other.vertices };
  indices = { other.indices };
  instances = { other.instances };

  return *this;
}

lpe::Renderer& lpe::Renderer::operator=(Renderer&& other)
{
  device = std::move(other.device);
  buffer = std::move(other.buffer);
  physicalDevice = other.physicalDevice;
  features = other.features;
  bufferId = other.bufferId;
  ubo = other.ubo;
  objects = std::move(other.objects);
  vertices = std::move(other.vertices);
  indices = std::move(other.indices);
  instances = std::move(other.instances);

  return *this;
}

lpe::Renderer::Renderer(vk::Device* device,
                        vk::PhysicalDevice physicalDevice,
                        lpe::BufferMemory* bufferMemory,
                        uint32_t bufferId)
  : physicalDevice(physicalDevice),
    bufferId(bufferId)
{
  this->device.reset(device);
  this->buffer.reset(bufferMemory);
  this->features = physicalDevice.getFeatures();
  this->ubo = {};
}

void lpe::Renderer::AddObject(ObjectRef obj)
{
  auto vertexOffset = (int32_t)vertices.size();
  auto indexOffset = (uint32_t)indices.size();

  obj->SetOffsets(indexOffset,
                  vertexOffset);

  if (objects.find(obj->GetPrio()) == std::end(objects))
  {
    objects.insert(std::make_pair(obj->GetPrio(),
                                  std::vector<ObjectRef> {}));
  }

  objects[obj->GetPrio()].push_back(obj);

  this->vertices.insert(std::end(this->vertices),
                        obj->GetVertexBegin(),
                        obj->GetVertexEnd());
  this->indices.insert(std::end(this->indices),
                       obj->GetIndexBegin(),
                       obj->GetIndexEnd());
}

void lpe::Renderer::UpdateBuffer(const lpe::Camera& camera)
{
  ubo.view = camera.GetView();
  ubo.projection = camera.GetPerspective();
  ubo.projection[1][1] *= -1;

  commands.clear();
  instances.clear();

  for (const auto& object : objects)
  {
    for (const auto obj : object.second)
    {
      commands.push_back(obj->GetIndirectCommand((uint32_t)instances.size()));

      auto instanceData = obj->GetInstanceData();
      instances.insert(std::end(instances),
                       std::begin(instanceData),
                       std::end(instanceData));
    }
  }

  vk::DeviceSize indexSize = sizeof(uint32_t) * indices.size();
  vk::DeviceSize vertexSize = sizeof(lpe::Vertex) * vertices.size();
  vk::DeviceSize indirectSize = sizeof(vk::DrawIndexedIndirectCommand) * commands.size();
  vk::DeviceSize instancesSize = sizeof(InstanceData) * instances.size();

  vk::DeviceSize bufferIndexSize = buffer->GetSize(bufferId,
                                                   BufferMemory::Type::Index);
  vk::DeviceSize bufferVertexSize = buffer->GetSize(bufferId,
                                                    BufferMemory::Type::Vertex);
  vk::DeviceSize bufferIndirectSize = buffer->GetSize(bufferId,
                                                      BufferMemory::Type::Indirect);
  vk::DeviceSize bufferInstanceSize = buffer->GetSize(bufferId,
                                                      BufferMemory::Type::Instance);

  // if memoryusage succeeds the available memory reallocate with the doubled needed size
  // this may get out of hands if the usage gets really high
  if (indexSize >= bufferIndexSize ||
    vertexSize >= bufferVertexSize ||
    indirectSize >= bufferIndirectSize ||
    instancesSize >= bufferInstanceSize)
  {
    lpe::BufferMemoryCreateInfo<1> createInfo = {};
    createInfo.ids = { bufferId };
    createInfo.usages = { buffer->GetUsageFlags(bufferId) };
    createInfo.propertyFlags = buffer->GetMemoryPropertyFlags();

    auto offsets = buffer->GetOffsets(bufferId);
    offsets[BufferMemory::Type::Vertex] = vertexSize * 2 * sizeof(lpe::Vertex);
    offsets[BufferMemory::Type::Index] = indexSize * 2 * sizeof(lpe::Vertex);
    offsets[BufferMemory::Type::Instance] = instancesSize * 2 * sizeof(lpe::Vertex);
    offsets[BufferMemory::Type::Indirect] = indirectSize * 2 * sizeof(lpe::Vertex);
    createInfo.offsets = { offsets };

    buffer->Recreate(createInfo);

    features = physicalDevice.getFeatures();
  }

  buffer->Write(&ubo,
                sizeof(UniformBufferObject),
                buffer->GetOffset(bufferId,
                                  BufferMemory::Type::UBO));

  buffer->Write(indices.data(),
                indexSize,
                buffer->GetOffset(bufferId,
                                  BufferMemory::Type::Index));
  buffer->Write(vertices.data(),
                vertexSize,
                buffer->GetOffset(bufferId,
                                  BufferMemory::Type::Vertex));
  buffer->Write(instances.data(),
                instancesSize,
                buffer->GetOffset(bufferId,
                                  BufferMemory::Type::Instance));
  buffer->Write(commands.data(),
                indirectSize,
                buffer->GetOffset(bufferId,
                                  BufferMemory::Type::Indirect));
}

void lpe::Renderer::Record(uint32_t prio,
                           vk::CommandBuffer cmd)
{
  vk::Buffer buffer = this->buffer->GetBuffer(bufferId);

  // bind Vertex
  std::array<vk::DeviceSize, 1> offsets = {
    this->buffer->GetOffset(bufferId,
                            BufferMemory::Type::Vertex)
  };
  cmd.bindVertexBuffers(0,
                        1,
                        &buffer,
                        offsets.data());

  // bind Instances
  offsets[0] = this->buffer->GetOffset(bufferId,
                                       BufferMemory::Type::Instance);
  cmd.bindVertexBuffers(1,
                        1,
                        &buffer,
                        offsets.data());

  // bind index
  cmd.bindIndexBuffer(buffer,
                      this->buffer->GetOffset(bufferId,
                                              BufferMemory::Type::Index),
                      vk::IndexType::eUint32);
  
  if (features.multiDrawIndirect)
  {
    // @formatter:off
    vk::DeviceSize offset = this->buffer->GetOffset(bufferId,
                                                    BufferMemory::Type::Indirect) + GetOffet(prio) * sizeof(vk::DrawIndexedIndirectCommand);
    // @formatter:on

    cmd.drawIndexedIndirect(buffer,
                            offset,
                            GetCount(prio),
                            sizeof(vk::DrawIndexedIndirectCommand));
  }
  else
  {
    for (size_t j = 0; j < commands.size(); j++)
    {
      cmd.drawIndexedIndirect(buffer,
                              j * sizeof(vk::DrawIndexedIndirectCommand),
                              1,
                              sizeof(vk::DrawIndexedIndirectCommand));
    }
  }
}

uint32_t lpe::Renderer::GetOffet(uint32_t prio) const
{
  uint32_t offset = 0;

  for (const auto& object : objects)
  {
    if (object.first == prio)
    {
      break;
    }

    offset++;
  }

  return offset;
}

uint32_t lpe::Renderer::GetCount(uint32_t prio) const
{
  auto found = objects.find(prio);
  if (found != std::end(objects))
  {
    return (*found).second.size();
  }

  return 0;
}

bool lpe::Renderer::Empty() const
{
  return objects.empty();
}

vk::DescriptorBufferInfo lpe::Renderer::GetBufferInfo()
{
  return buffer->SetupDescriptor(bufferId,
                                 buffer->GetSize(bufferId,
                                                 BufferMemory::Type::UBO),
                                 buffer->GetOffset(bufferId,
                                                   BufferMemory::Type::UBO));
}

void lpe::Renderer::SetLightPosition(glm::vec3 pos)
{
  ubo.lightPos = pos;
}
