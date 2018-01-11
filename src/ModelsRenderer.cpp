#include "../include/ModelsRenderer.h"

void lpe::ModelsRenderer::Copy(const ModelsRenderer& other)
{
  this->commands.reset(other.commands.get());
  this->indices = { other.indices };
  this->vertices = { other.vertices };
  this->objects = { other.objects };
  this->indexBuffer = { other.indexBuffer };
  this->vertexBuffer = { other.vertexBuffer };
	this->indirectBuffer = { other.indirectBuffer };
}

void lpe::ModelsRenderer::Move(ModelsRenderer& other)
{
  this->commands = std::move(other.commands);
  this->indices = std::move(other.indices);
  this->vertices = std::move(other.vertices);
  this->indexBuffer = std::move(other.indexBuffer);
  this->vertexBuffer = std::move(other.vertexBuffer);
	this->indirectBuffer = std::move(other.indirectBuffer);
}

lpe::ModelsRenderer::ModelsRenderer(const ModelsRenderer& other)
{
  Copy(other);
}

lpe::ModelsRenderer::ModelsRenderer(ModelsRenderer&& other) noexcept
{
  Move(other);
}

lpe::ModelsRenderer& lpe::ModelsRenderer::operator=(const ModelsRenderer& other)
{
  Copy(other);
  return *this;
}

lpe::ModelsRenderer& lpe::ModelsRenderer::operator=(ModelsRenderer&& other) noexcept
{
  Move(other);
  return *this;
}

lpe::ModelsRenderer::ModelsRenderer(vk::PhysicalDevice physicalDevice, vk::Device* device, Commands* commands)
{
  this->physicalDevice = physicalDevice;
  this->device.reset(device);
  this->commands.reset(commands);

  indexBuffer = { physicalDevice, device };
  vertexBuffer = { physicalDevice, device };
	indirectBuffer = { physicalDevice, device };
}

lpe::ModelsRenderer::~ModelsRenderer()
{
  if(commands)
  {
    commands.release();
  }

  if(device)
  {
    device.release();
  }
}

std::vector<vk::DrawIndexedIndirectCommand> lpe::ModelsRenderer::GetDrawIndexedIndirectCommands(uint32_t prio)
{
	std::vector<vk::DrawIndexedIndirectCommand> commands = {};

	uint32_t i = 0;
	for (auto& entry : objects)
	{
    if (entry->GetPrio() == prio)
    {
      commands.push_back(entry->GetIndirectCommand(i));
      i += entry->GetInstanceCount();
    }
	}

	return commands;
}

std::vector<lpe::InstanceData> lpe::ModelsRenderer::GetInstanceData() const
{
  std::vector<lpe::InstanceData> instances;

  for (const auto& entry : objects)
  {
    auto instanceData = entry->GetInstanceData();
    instances.insert(std::end(instances), std::begin(instanceData), std::end(instanceData));
  }

  return instances;
}

void lpe::ModelsRenderer::AddObject(ObjectRef obj)
{
  int32_t vertexOffset = (int32_t)vertices.size();
  uint32_t indexOffset = (uint32_t)indices.size();

  obj->SetOffsets(indexOffset, vertexOffset);

	objects.push_back(obj);

	this->vertices.insert(std::end(this->vertices), obj->GetVertexBegin(), obj->GetVertexEnd());
	this->indices.insert(std::end(this->indices), obj->GetIndexBegin(), obj->GetIndexEnd());

	UpdateBuffer();
}


void lpe::ModelsRenderer::UpdateBuffer()
{
  vk::DeviceSize indexSize = sizeof(indices[0]) * indices.size();
  vk::DeviceSize vertexSize = sizeof(vertices[0]) * vertices.size();
 
  /*indexBuffer = commands->CreateBuffer(indices.data(), indexSize);
  vertexBuffer = commands->CreateBuffer(vertices.data(), vertexSize);*/

  vertexBuffer.CreateStaged(*commands, vertexSize, vertices.data(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
  indexBuffer.CreateStaged(*commands, indexSize, indices.data(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
}

uint32_t lpe::ModelsRenderer::GetCount() const
{
  return (uint32_t)objects.size();
}

std::vector<lpe::Vertex> lpe::ModelsRenderer::GetVertices() const
{
  return vertices;
}

std::vector<uint32_t> lpe::ModelsRenderer::GetIndices() const
{
  return indices;
}

vk::Buffer lpe::ModelsRenderer::GetVertexBuffer()
{
  return vertexBuffer.GetBuffer();
}

vk::Buffer lpe::ModelsRenderer::GetIndexBuffer()
{
  return indexBuffer.GetBuffer();
}

bool lpe::ModelsRenderer::Empty() const
{
  return objects.empty();
}

uint32_t lpe::ModelsRenderer::EntriesCount() const
{
  return (uint32_t)objects.size();
}

vk::Buffer lpe::ModelsRenderer::GetIndirectBuffer(uint32_t prio)
{
  auto cmds = GetDrawIndexedIndirectCommands(prio);
  vk::DeviceSize indirectSize = cmds.size() * sizeof(vk::DrawIndexedIndirectCommand);

  if (indirectSize >= indirectBuffer.GetSize() || !indirectBuffer.GetBuffer())
  {
    indirectBuffer.CreateStaged(*commands, 
                                indirectSize, 
                                cmds.data(), 
                                vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndirectBuffer, 
                                vk::MemoryPropertyFlagBits::eDeviceLocal);
  }
  else
  {
    auto cmdBuffer = commands->BeginSingleTimeCommands();

    indirectBuffer.CopyStaged(cmdBuffer, cmds.data());

    commands->EndSingleTimeCommands(cmdBuffer);
  }

	return indirectBuffer.GetBuffer();
}
