#include "RenderObject.h"
#include "ServiceLocator.h"

lpe::rendering::Texture::Texture(const Texture& other)
{
  this->color = other.color;
  this->image = other.image;
}

lpe::rendering::Texture::Texture(Texture&& other) noexcept
{
  this->color = other.color;
  this->image = std::move(other.image);
}

lpe::rendering::Texture& lpe::rendering::Texture::operator=(Texture&& other) noexcept
{
  this->color = other.color;
  this->image = std::move(other.image);
  return *this;
}

lpe::rendering::Texture::~Texture()
{
  image.reset();
}

void lpe::rendering::Texture::LoadImage(const lpe::utils::Uuid& uuid)
{
  auto resourceManager = ServiceLocator::ResourceManager.Get()
                                                        .lock();
  assert(resourceManager);

  image = resourceManager->Get(uuid);
}

void lpe::rendering::Texture::LoadImage(const char* fileName)
{
  auto resourceManager = ServiceLocator::ResourceManager.Get()
                                                        .lock();
  assert(resourceManager);

  image = resourceManager->Load(fileName);
}

void lpe::rendering::Texture::SetImage(std::weak_ptr<lpe::utils::Resource>&& image)
{
  this->image = image;
}

void lpe::rendering::Texture::SetImage(const std::weak_ptr<lpe::utils::Resource>& image)
{
  this->image = image;
}

std::weak_ptr<lpe::utils::Resource> lpe::rendering::Texture::GetImage() const
{
  return image;
}

void lpe::rendering::Texture::SetColor(glm::vec4 color)
{
  this->color = color;
}

glm::vec4 lpe::rendering::Texture::GetColor() const
{
  return color;
}

lpe::rendering::Material::Material(const Material& other)
{
  this->albedo = other.albedo;
  this->normal = other.normal;
  this->environment = other.environment;
}

lpe::rendering::Material::Material(Material&& other) noexcept
{
  this->albedo = std::move(other.albedo);
  this->normal = std::move(other.normal);
  this->environment = std::move(other.environment);
}

lpe::rendering::Material& lpe::rendering::Material::operator=(const Material& other)
{
  this->albedo = other.albedo;
  this->normal = other.normal;
  this->environment = other.environment;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::operator=(Material&& other) noexcept
{
  this->albedo = std::move(other.albedo);
  this->normal = std::move(other.normal);
  this->environment = std::move(other.environment);

  return *this;
}

lpe::rendering::Material::~Material()
{
  this->albedo.reset();
  this->normal.reset();
  for (auto&& ptr : this->environment)
  {
    ptr.reset();
  }
}

lpe::rendering::Material& lpe::rendering::Material::SetAlbedo(std::weak_ptr<Texture>&& albedo)
{
  this->albedo = albedo;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetAlbedo(const std::weak_ptr<Texture>& albedo)
{
  this->albedo = albedo;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetNormal(std::weak_ptr<Texture>&& normal)
{
  this->normal = normal;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetNormal(const std::weak_ptr<Texture>& normal)
{
  this->normal = normal;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetEnvironmentXP(std::weak_ptr<Texture>&& xp)
{
  this->environment[0] = xp;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetEnvironmentXN(const std::weak_ptr<Texture>& xn)
{
  this->environment[1] = xn;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetEnvironmentYP(std::weak_ptr<Texture>&& yp)
{
  this->environment[2] = yp;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetEnvironmentYN(const std::weak_ptr<Texture>& yn)
{
  this->environment[3] = yn;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetEnvironmentZP(std::weak_ptr<Texture>&& zp)
{
  this->environment[4] = zp;

  return *this;
}

lpe::rendering::Material& lpe::rendering::Material::SetEnvironmentZN(const std::weak_ptr<Texture>& zn)
{
  this->environment[5] = zn;

  return *this;
}

lpe::rendering::RenderTarget::RenderTarget(const RenderTarget& other)
{
  this->mesh = other.mesh;
  this->material = other.material;
  this->vertexShader = other.vertexShader;
  this->geometryShader = other.geometryShader;
  this->tessEvalShader = other.tessEvalShader;
  this->tessControlShader = other.tessControlShader;
  this->fragmentShader = other.fragmentShader;
  this->position = other.position;
  this->matrix = other.matrix;
}

lpe::rendering::RenderTarget::RenderTarget(RenderTarget&& other) noexcept
{
  this->mesh = std::move(other.mesh);
  this->material = std::move(other.material);
  this->vertexShader = std::move(other.vertexShader);
  this->geometryShader = std::move(other.geometryShader);
  this->tessEvalShader = std::move(other.tessEvalShader);
  this->tessControlShader = std::move(other.tessControlShader);
  this->fragmentShader = std::move(other.fragmentShader);
  this->position = other.position;
  this->matrix = other.matrix;
}

lpe::rendering::RenderTarget& lpe::rendering::RenderTarget::operator=(RenderTarget&& other) noexcept
{
  this->mesh = std::move(other.mesh);
  this->material = std::move(other.material);
  this->vertexShader = std::move(other.vertexShader);
  this->geometryShader = std::move(other.geometryShader);
  this->tessEvalShader = std::move(other.tessEvalShader);
  this->tessControlShader = std::move(other.tessControlShader);
  this->fragmentShader = std::move(other.fragmentShader);
  this->position = other.position;
  this->matrix = other.matrix;

  return *this;
}

lpe::rendering::RenderTarget::~RenderTarget()
{
  this->mesh.reset();
  this->material.reset();
  this->vertexShader.reset();
  this->geometryShader.reset();
  this->tessEvalShader.reset();
  this->tessControlShader.reset();
  this->fragmentShader.reset();
}

void lpe::rendering::RenderTarget::SetMesh(RenderResource&& mesh)
{
  this->mesh = mesh;
}

void lpe::rendering::RenderTarget::SetMesh(const RenderResource& mesh)
{
  this->mesh = mesh;
}

void lpe::rendering::RenderTarget::SetMaterial(std::weak_ptr<Material>&& material)
{
  this->material = material;
}

void lpe::rendering::RenderTarget::SetMaterial(const std::weak_ptr<Material>& material)
{
  this->material = material;
}

void lpe::rendering::RenderTarget::SetVertexShader(RenderResource&& vertexShader)
{
  this->vertexShader = vertexShader;
}

void lpe::rendering::RenderTarget::SetVertexShader(const RenderResource& vertexShader)
{
  this->vertexShader = vertexShader;
}

void lpe::rendering::RenderTarget::SetGeometryShader(RenderResource&& geometryShader)
{
  this->geometryShader = geometryShader;
}

void lpe::rendering::RenderTarget::SetGeometryShader(const RenderResource& geometryShader)
{
  this->geometryShader = geometryShader;
}

void lpe::rendering::RenderTarget::SetTessEvalShader(RenderResource&& tessEvalShader)
{
  this->tessEvalShader = tessEvalShader;
}

void lpe::rendering::RenderTarget::SetTessEvalShader(const RenderResource& tessEvalShader)
{
  this->tessEvalShader = tessEvalShader;
}

void lpe::rendering::RenderTarget::SetTessControlShader(RenderResource&& tessControlShader)
{
  this->tessControlShader = tessControlShader;
}

void lpe::rendering::RenderTarget::SetTessControlShader(const RenderResource& tessControlShader)
{
  this->tessControlShader = tessControlShader;
}

void lpe::rendering::RenderTarget::SetFragmentShader(RenderResource&& fragmentShader)
{
  this->fragmentShader = fragmentShader;
}

void lpe::rendering::RenderTarget::SetFragmentShader(const RenderResource& fragmentShader)
{
  this->fragmentShader = fragmentShader;
}

void lpe::rendering::RenderTarget::SetTransform(glm::mat4 transform)
{
  this->matrix = transform;
}

glm::mat4 lpe::rendering::RenderTarget::Transform(glm::mat4 transform)
{
  this->matrix *= transform;

  return this->matrix;
}

void lpe::rendering::RenderTarget::SetPosition(glm::vec3 pos)
{
  this->position = pos;
}

void lpe::rendering::RenderTarget::Move(glm::vec3 delta)
{
  this->position += delta;
}

glm::mat4 lpe::rendering::RenderTarget::GetTransform(glm::mat4 p) const
{
  return p * matrix;
}

glm::vec3 lpe::rendering::RenderTarget::GetPosition(glm::vec3 p) const
{
  return p + position;
}

void lpe::rendering::RenderTarget::SetUuid(const lpe::utils::Uuid & uuid)
{
  if(!this->uuid)
  {
    this->uuid = uuid;
  }
}

lpe::utils::Uuid lpe::rendering::RenderTarget::GetUuid() const
{
  return uuid;
}

lpe::rendering::RenderObject::RenderObject(const RenderObject& other)
{
}

lpe::rendering::RenderObject::RenderObject(RenderObject&& other) noexcept
{
}

lpe::rendering::RenderObject& lpe::rendering::RenderObject::operator=(RenderObject&& other) noexcept
{
  return *this;
}

lpe::rendering::RenderObject::~RenderObject()
{
  parts.clear();
}

void lpe::rendering::RenderObject::SetTransform(glm::mat4 transform)
{
}

glm::mat4 lpe::rendering::RenderObject::Transform(glm::mat4 transform)
{
  return glm::mat4(1.0);
}

void lpe::rendering::RenderObject::SetPosition(glm::vec3 pos)
{
}

void lpe::rendering::RenderObject::Move(glm::vec3 delta)
{
}

void lpe::rendering::RenderObject::AddTarget(const std::weak_ptr<RenderTarget>& target)
{
  auto ptr = target.lock();

  assert(ptr);
  auto uuid = ptr->GetUuid();

  if(!uuid)
  {
    uuid = lpe::utils::Uuid::GetNew();
    parts.insert(std::make_pair(uuid, std::vector<std::weak_ptr<RenderTarget>>()));
  }

  parts[uuid].emplace_back(ptr);
}

void lpe::rendering::RenderObject::AddTarget(std::weak_ptr<RenderTarget>&& target)
{
  this->AddTarget(target);
}

