#include "ModelsRenderer.h"

bool lpe::ModelsRenderer::Entry::operator<(const Entry& e) const
{
  return model < e.model;
}

bool lpe::ModelsRenderer::Entry::operator==(const Entry& e) const
{
  return e.model == model;
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

lpe::ModelsRenderer::ModelsRenderer(Commands* commands)
{
  this->commands.reset(commands);
}

lpe::ModelsRenderer::~ModelsRenderer()
{
  if(commands)
  {
    commands.release();
  }
}

void lpe::ModelsRenderer::AddObject(Model* model)
{
  auto vertices = model->GetVertices();

  Entry e = {};
  e.model.reset(model);
  e.indicesLength = (uint32_t)model->GetIndices().size();
  e.verticesLength = (uint32_t)model->GetVertices().size();
  e.verticesStartIndex = (uint32_t)this->vertices.size();
  e.indicesStartIndex = (uint32_t)indices.size();

  entries.insert(e);

  this->vertices.insert(std::end(this->vertices), std::begin(vertices), std::end(vertices));
  auto indices = model->GetIndices((uint32_t)this->indices.size());
  this->indices.insert(std::end(this->indices), std::begin(indices), std::end(indices));

  UpdateBuffer();
}

void lpe::ModelsRenderer::RemoveObject(Model* model)
{
  Entry e = {};
  e.model.reset(model);

  std::set<Entry>::iterator find = entries.find(e);

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

  indexBuffer = commands->CreateBuffer(indices.data(), indexSize);
  vertexBuffer = commands->CreateBuffer(vertices.data(), vertexSize);
}

std::vector<lpe::Model> lpe::ModelsRenderer::GetModels() const
{
  std::vector<lpe::Model> models = {};

  std::for_each(entries.begin(), entries.end(), [models = &models](const Entry& item) { models->push_back(*item.model); });

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
