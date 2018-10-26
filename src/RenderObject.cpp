#include "RenderObject.h"
#include "ServiceLocator.h"

lpe::render::Texture::Texture(const Texture& other)
{
  this->color = other.color;
  this->image = other.image;
}

lpe::render::Texture::Texture(Texture&& other) noexcept
{
  this->color = other.color;
  this->image = std::move(other.image);
}

lpe::render::Texture& lpe::render::Texture::operator=(Texture&& other) noexcept
{
  this->color = other.color;
  this->image = std::move(other.image);
  return *this;
}

lpe::render::Texture::~Texture()
{
  image.reset();
}

void lpe::render::Texture::LoadImage(const lpe::utils::Uuid& uuid)
{
  auto resourceManager = ServiceLocator::ResourceManager.Get()
                                                        .lock();
  assert(resourceManager);

  image = resourceManager->Get(uuid);
}

void lpe::render::Texture::LoadImage(const char* fileName)
{
  auto resourceManager = ServiceLocator::ResourceManager.Get()
                                                        .lock();
  assert(resourceManager);

  image = resourceManager->Load(fileName);
}

void lpe::render::Texture::SetImage(std::weak_ptr<lpe::utils::Resource>&& image)
{
  this->image = image;
}

void lpe::render::Texture::SetImage(const std::weak_ptr<lpe::utils::Resource>& image)
{
  this->image = image;
}

std::weak_ptr<lpe::utils::Resource> lpe::render::Texture::GetImage() const
{
  return image;
}

void lpe::render::Texture::SetColor(glm::vec4 color)
{
  this->color = color;
}

glm::vec4 lpe::render::Texture::GetColor() const
{
  return color;
}

lpe::render::Material::Material(const Material& other)
{
  this->albedo = other.albedo;
  this->normal = other.normal;
  this->environment = other.environment;
}

lpe::render::Material::Material(Material&& other) noexcept
{
  this->albedo = std::move(other.albedo);
  this->normal = std::move(other.normal);
  this->environment = std::move(other.environment);
}

lpe::render::Material& lpe::render::Material::operator=(const Material& other)
{
  this->albedo = other.albedo;
  this->normal = other.normal;
  this->environment = other.environment;

  return *this;
}

lpe::render::Material& lpe::render::Material::operator=(Material&& other) noexcept
{
  this->albedo = std::move(other.albedo);
  this->normal = std::move(other.normal);
  this->environment = std::move(other.environment);

  return *this;
}

lpe::render::Material::~Material()
{
  this->albedo.reset();
  this->normal.reset();
  for (auto&& ptr : this->environment)
  {
    ptr.reset();
  }
}

lpe::render::Material& lpe::render::Material::SetAlbedo(std::weak_ptr<Texture>&& albedo)
{
  this->albedo = albedo;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetAlbedo(const std::weak_ptr<Texture>& albedo)
{
  this->albedo = albedo;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetNormal(std::weak_ptr<Texture>&& normal)
{
  this->normal = normal;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetNormal(const std::weak_ptr<Texture>& normal)
{
  this->normal = normal;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetEnvironmentXP(std::weak_ptr<Texture>&& xp)
{
  this->environment[0] = xp;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetEnvironmentXN(const std::weak_ptr<Texture>& xn)
{
  this->environment[1] = xn;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetEnvironmentYP(std::weak_ptr<Texture>&& yp)
{
  this->environment[2] = yp;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetEnvironmentYN(const std::weak_ptr<Texture>& yn)
{
  this->environment[3] = yn;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetEnvironmentZP(std::weak_ptr<Texture>&& zp)
{
  this->environment[4] = zp;

  return *this;
}

lpe::render::Material& lpe::render::Material::SetEnvironmentZN(const std::weak_ptr<Texture>& zn)
{
  this->environment[5] = zn;

  return *this;
}

lpe::render::RenderTarget::RenderTarget(const RenderTarget& other)
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

lpe::render::RenderTarget::RenderTarget(RenderTarget&& other) noexcept
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

lpe::render::RenderTarget& lpe::render::RenderTarget::operator=(RenderTarget&& other) noexcept
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

lpe::render::RenderTarget::~RenderTarget()
{
  this->mesh.reset();
  this->material.reset();
  this->vertexShader.reset();
  this->geometryShader.reset();
  this->tessEvalShader.reset();
  this->tessControlShader.reset();
  this->fragmentShader.reset();
}

void lpe::render::RenderTarget::SetMesh(RenderResource&& mesh)
{
  this->mesh = mesh;
}

void lpe::render::RenderTarget::SetMesh(const RenderResource& mesh)
{
  this->mesh = mesh;
}

void lpe::render::RenderTarget::SetMaterial(std::weak_ptr<Material>&& material)
{
  this->material = material;
}

void lpe::render::RenderTarget::SetMaterial(const std::weak_ptr<Material>& material)
{
  this->material = material;
}

void lpe::render::RenderTarget::SetVertexShader(RenderResource&& vertexShader)
{
  this->vertexShader = vertexShader;
}

void lpe::render::RenderTarget::SetVertexShader(const RenderResource& vertexShader)
{
  this->vertexShader = vertexShader;
}

void lpe::render::RenderTarget::SetGeometryShader(RenderResource&& geometryShader)
{
  this->geometryShader = geometryShader;
}

void lpe::render::RenderTarget::SetGeometryShader(const RenderResource& geometryShader)
{
  this->geometryShader = geometryShader;
}

void lpe::render::RenderTarget::SetTessEvalShader(RenderResource&& tessEvalShader)
{
  this->tessEvalShader = tessEvalShader;
}

void lpe::render::RenderTarget::SetTessEvalShader(const RenderResource& tessEvalShader)
{
  this->tessEvalShader = tessEvalShader;
}

void lpe::render::RenderTarget::SetTessControlShader(RenderResource&& tessControlShader)
{
  this->tessControlShader = tessControlShader;
}

void lpe::render::RenderTarget::SetTessControlShader(const RenderResource& tessControlShader)
{
  this->tessControlShader = tessControlShader;
}

void lpe::render::RenderTarget::SetFragmentShader(RenderResource&& fragmentShader)
{
  this->fragmentShader = fragmentShader;
}

void lpe::render::RenderTarget::SetFragmentShader(const RenderResource& fragmentShader)
{
  this->fragmentShader = fragmentShader;
}

void lpe::render::RenderTarget::SetTransform(glm::mat4 transform)
{
  this->matrix = transform;
}

glm::mat4 lpe::render::RenderTarget::Transform(glm::mat4 transform)
{
  this->matrix *= transform;

  return this->matrix;
}

void lpe::render::RenderTarget::SetPosition(glm::vec3 pos)
{
  this->position = pos;
}

void lpe::render::RenderTarget::Move(glm::vec3 delta)
{
  this->position += delta;
}

glm::mat4 lpe::render::RenderTarget::GetTransform(glm::mat4 p) const
{
  return p * matrix;
}

glm::vec3 lpe::render::RenderTarget::GetPosition(glm::vec3 p) const
{
  return p + position;
}

void lpe::render::RenderTarget::SetUuid(const lpe::utils::Uuid & uuid)
{
  if(!this->uuid)
  {
    this->uuid = uuid;
  }
}

lpe::utils::Uuid lpe::render::RenderTarget::GetUuid() const
{
  return uuid;
}

lpe::render::RenderObject::RenderObject(const RenderObject& other)
{
}

lpe::render::RenderObject::RenderObject(RenderObject&& other) noexcept
{
}

lpe::render::RenderObject& lpe::render::RenderObject::operator=(RenderObject&& other) noexcept
{
  return *this;
}

lpe::render::RenderObject::~RenderObject()
{
  parts.clear();
}

void lpe::render::RenderObject::SetTransform(glm::mat4 transform)
{
}

glm::mat4 lpe::render::RenderObject::Transform(glm::mat4 transform)
{
  return glm::mat4(1.0);
}

void lpe::render::RenderObject::SetPosition(glm::vec3 pos)
{
}

void lpe::render::RenderObject::Move(glm::vec3 delta)
{
}

void lpe::render::RenderObject::AddTarget(const std::weak_ptr<RenderTarget>& target)
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

void lpe::render::RenderObject::AddTarget(std::weak_ptr<RenderTarget>&& target)
{
  this->AddTarget(target);
}

