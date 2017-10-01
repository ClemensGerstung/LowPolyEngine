#include "ModelsRenderer.h"

bool lpe::ModelsRenderer::Entry::operator==(const Entry& e)
{
	return model == e.model;
}

void lpe::ModelsRenderer::Copy(const ModelsRenderer& other)
{
  this->commands.reset(other.commands.get());
  this->indices = { other.indices };
  this->vertices = { other.vertices };
  this->entries = { other.entries };
  this->indexBuffer = { other.indexBuffer };
  this->vertexBuffer = { other.vertexBuffer };
}

void lpe::ModelsRenderer::Move(ModelsRenderer& other)
{
  this->commands = std::move(other.commands);
  this->indices = std::move(other.indices);
  this->vertices = std::move(other.vertices);
  this->indexBuffer = std::move(other.indexBuffer);
  this->vertexBuffer = std::move(other.vertexBuffer);
}

lpe::ModelsRenderer::ModelsRenderer(const ModelsRenderer& other)
{
  Copy(other);
}

lpe::ModelsRenderer::ModelsRenderer(ModelsRenderer&& other)
{
  Move(other);
}

lpe::ModelsRenderer& lpe::ModelsRenderer::operator=(const ModelsRenderer& other)
{
  Copy(other);
  return *this;
}

lpe::ModelsRenderer& lpe::ModelsRenderer::operator=(ModelsRenderer&& other)
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

void lpe::ModelsRenderer::AddObject(std::vector<lpe::Vertex> vertices, std::vector<uint32_t> indices)
{
  Entry e = {};
  e.model.SetIndices(indices);
  e.model.SetVertices(vertices);

  e.indicesLength = (uint32_t)e.model.GetIndices().size();
  e.verticesLength = (uint32_t)vertices.size();
  e.verticesStartIndex = (uint32_t)this->vertices.size();
  e.indicesStartIndex = (uint32_t)indices.size();

  entries.push_back(e);

  this->vertices.insert(std::end(this->vertices), std::begin(vertices), std::end(vertices));
  this->indices.insert(std::end(this->indices), std::begin(indices), std::end(indices));

  UpdateBuffer();
}

lpe::Model lpe::ModelsRenderer::operator[](uint32_t index) const
{
  return entries[index].model;
}

lpe::Model* lpe::ModelsRenderer::AddObject(std::string path)
{
	Entry e = {};
	e.model.Load(path);
	e.indicesLength = (uint32_t)e.model.GetIndices().size();

	std::vector<Vertex> vertices = e.model.GetVertices();
	auto indices = e.model.GetIndices((uint32_t)this->indices.size());

	e.verticesLength = (uint32_t)vertices.size();
	e.verticesStartIndex = (uint32_t)this->vertices.size();
	e.indicesStartIndex = (uint32_t)indices.size();

	entries.push_back(e);

	this->vertices.insert(std::end(this->vertices), std::begin(vertices), std::end(vertices));
	this->indices.insert(std::end(this->indices), std::begin(indices), std::end(indices));

	UpdateBuffer();

	return &(*(entries.end() - 1)).model;
}

void lpe::ModelsRenderer::RemoveObject(Model* model)
{
  Entry e = {};
  e.model = *model;

  auto find = std::find(entries.begin(), entries.end(), e);

  if(find != entries.end())
  {
    indices.erase(std::begin(indices) + find->indicesStartIndex, std::begin(indices) + find->indicesStartIndex + find->indicesLength);
    vertices.erase(std::begin(vertices) + find->verticesStartIndex, std::begin(vertices) + find->verticesStartIndex + find->verticesLength);

    entries.erase(find);

    UpdateBuffer();
  }
}

void lpe::ModelsRenderer::UpdateBuffer()
{
  vk::DeviceSize indexSize = sizeof(indices[0]) * indices.size();
  vk::DeviceSize vertexSize = sizeof(vertices[0]) * vertices.size();

  /*indexBuffer = commands->CreateBuffer(indices.data(), indexSize);
  vertexBuffer = commands->CreateBuffer(vertices.data(), vertexSize);*/

  indexBuffer.Create(*commands, indexSize, indices.data(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
  vertexBuffer.Create(*commands, vertexSize, vertices.data(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
}

std::vector<lpe::Model> lpe::ModelsRenderer::GetModels() const
{
  std::vector<lpe::Model> models = {};
  models.resize(entries.size());


  for (size_t i = 0; i < models.size(); ++i)
  {
    models[i] = { entries[i].model };
  }

  return models;
}

uint32_t lpe::ModelsRenderer::GetCount() const
{
  return (uint32_t)entries.size();
}

std::vector<lpe::Vertex> lpe::ModelsRenderer::GetVertices() const
{
  return vertices;
}

std::vector<uint32_t> lpe::ModelsRenderer::GetIndices() const
{
  return indices;
}

vk::Buffer* lpe::ModelsRenderer::GetVertexBuffer()
{
  return vertexBuffer.GetBuffer();
}

vk::Buffer* lpe::ModelsRenderer::GetIndexBuffer()
{
  return indexBuffer.GetBuffer();
}

bool lpe::ModelsRenderer::Empty() const
{
  return entries.empty();
}

uint32_t lpe::ModelsRenderer::EntriesCount() const
{
  return (uint32_t)entries.size();
}
