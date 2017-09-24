#include "Model.h"
#include "fstream"
#include "sstream"

void lpe::Model::Copy(const Model& other)
{
  this->position = other.position;
  this->matrix = other.matrix;
  this->indices = { other.indices };
  this->vertices = { other.vertices };
}

void lpe::Model::Move(Model& other)
{
  this->position = other.position;
  this->matrix = other.matrix;
  this->indices = std::move(other.indices);
  this->vertices = std::move(other.vertices);
}

lpe::Model::Model(const Model& other)
{
  Copy(other);
}

lpe::Model::Model(Model&& other)
{
  Move(other);
}

lpe::Model& lpe::Model::operator=(const Model& other)
{
  Copy(other);
  return *this;
}

lpe::Model& lpe::Model::operator=(Model&& other)
{
  Move(other);
  return *this;
}

void lpe::Model::Load(std::string fileName)
{
  // simply load ply file
  // ignoring header!

  std::ifstream file(fileName);

  if(!file)
  {
    std::runtime_error("could not open model " + fileName);
  }

  std::string line;
  bool header = true;
  uint32_t countVertices = -1;
  uint32_t countFaces = -1;

  while (std::getline(file, line))
  {
    if(header)
    {
      int32_t find = (int32_t)line.find("element vertex");
      if(find >= 0)
      {
        countVertices = std::stoi(line.substr(line.find_last_of(" ") + 1));
        continue;
      }

      find = (int32_t)line.find("element face");
      if (find >= 0)
      {
        countFaces = std::stoi(line.substr(line.find_last_of(" ") + 1));
        continue;
      }

      if(line == "end_header")
      {
        header = false;
        break;
      }
    }
  }

  if(header || countVertices == -1 || countFaces == -1)
  {
    throw std::runtime_error("file doesn't have the right format");
  }

  vertices.resize(countVertices);
  for (uint32_t i = 0; i < countVertices; i++)
  {
    std::getline(file, line);

    std::istringstream linepart(line);
    std::string part;
    uint16_t index = 0;
    Vertex v;

    while(std::getline(linepart, part, ' '))
    {
      if(index <= 2)
      {
        v.position[index] = std::stof(part);
      }
      else if(index <= 5)
      {
        v.normals[index % 3] = std::stof(part);
      }
      else if(index <= 8)
      {
        v.color[index % 3] = std::stof(part) / 255.f;
      }
      else
      {
        break;
      }
      index++;
    }

    vertices[i] = v;
  }

  indices.resize(countFaces * 3);
  for (uint32_t i = 0; i < countFaces; i++)
  {
    std::getline(file, line);

    std::istringstream linepart(line);
    std::string part;
    uint16_t index = 0;

    while (std::getline(linepart, part, ' '))
    {
      if (index == 0 && std::stoi(part) != 3)
      {
        throw std::runtime_error("faces are not triangulated!");
      }
      
      if(index > 0)
      {
        indices[(i * 3 + index - 1)] = std::stoi(part);
      }

      index++;
    }
  }

  file.close();
}

void lpe::Model::SetPosition(glm::vec3 position)
{
  this->position = position;
}

void lpe::Model::Move(glm::vec3 offset)
{
  position += offset;
}

void lpe::Model::Transform(glm::mat4 transform)
{
  this->matrix = transform;
}

glm::vec3 lpe::Model::GetPosition() const
{
  return position;
}

glm::mat4 lpe::Model::GetModelMatrix() const
{
  return matrix;
}

std::vector<lpe::Vertex> lpe::Model::GetVertices() const
{
  return vertices;
}

std::vector<uint32_t> lpe::Model::GetIndices(uint32_t offset)
{
  if(offset == 0)
  {
    return indices;
  }

  std::vector<uint32_t> copy = {indices};

  std::for_each(copy.begin(), copy.end(), [offset = offset](uint32_t& index) { index += offset; });

  return copy;
}

bool lpe::Model::operator==(const Model& model)
{
	return position == model.position && matrix == model.matrix && vertices == model.vertices && indices == model.indices;
}
