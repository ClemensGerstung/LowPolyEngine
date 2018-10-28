#pragma once

#include "Resource.h"
#include <glm/glm.hpp>
#include <map>
#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace rendering
  {
    class VkMemoryManagement;

    class Texture
    {
    private:
      glm::vec4 color;
      std::weak_ptr<lpe::utils::Resource> image;
    public:
      Texture() = default;
      Texture(const Texture& other);
      Texture(Texture&& other) noexcept;
      Texture& operator=(const Texture& other) = default;
      Texture& operator=(Texture&& other) noexcept;
      ~Texture();

      void LoadImage(const lpe::utils::Uuid& uuid);
      void LoadImage(const char* fileName);
      void SetImage(std::weak_ptr<lpe::utils::Resource>&& image);
      void SetImage(const std::weak_ptr<lpe::utils::Resource>& image);
      std::weak_ptr<lpe::utils::Resource> GetImage() const;

      void SetColor(glm::vec4 color);
      glm::vec4 GetColor() const;
    };



    class Material
    {
    private:
      std::weak_ptr<Texture> albedo;
      std::weak_ptr<Texture> normal;
      std::weak_ptr<Texture> light;
      std::weak_ptr<Texture> displacement;

      // +x, -x, +y, -y, +z, -z
      std::array<std::weak_ptr<Texture>, 6> environment;
    public:
      Material() = default;
      Material(const Material& other);
      Material(Material&& other) noexcept;
      Material& operator=(const Material& other);
      Material& operator=(Material&& other) noexcept;
      ~Material();

      Material& SetAlbedo(std::weak_ptr<Texture>&& albedo);
      Material& SetAlbedo(const std::weak_ptr<Texture>& albedo);
      Material& SetNormal(std::weak_ptr<Texture>&& normal);
      Material& SetNormal(const std::weak_ptr<Texture>& normal);
      Material& SetEnvironmentXP(std::weak_ptr<Texture>&& xp);
      Material& SetEnvironmentXN(const std::weak_ptr<Texture>& xn);
      Material& SetEnvironmentYP(std::weak_ptr<Texture>&& yp);
      Material& SetEnvironmentYN(const std::weak_ptr<Texture>& yn);
      Material& SetEnvironmentZP(std::weak_ptr<Texture>&& zp);
      Material& SetEnvironmentZN(const std::weak_ptr<Texture>& zn);

      void Create(const VkMemoryManagement& memory);
    };

    class RenderTarget
    {
    private:
      using RenderResource = std::weak_ptr<lpe::utils::Resource>;

      lpe::utils::Uuid uuid;
      RenderResource mesh;
      std::weak_ptr<Material> material;
      RenderResource vertexShader;
      RenderResource geometryShader;
      RenderResource tessEvalShader;
      RenderResource tessControlShader;
      RenderResource fragmentShader;

      glm::vec3 position;
      glm::mat4 matrix;
    public:
      RenderTarget() = default;
      RenderTarget(const RenderTarget& other);
      RenderTarget(RenderTarget&& other) noexcept;
      RenderTarget& operator=(const RenderTarget& other) = default;
      RenderTarget& operator=(RenderTarget&& other) noexcept;
      ~RenderTarget();

      void SetMesh(RenderResource&& mesh);
      void SetMesh(const RenderResource& mesh);
      void SetMaterial(std::weak_ptr<Material>&& material);
      void SetMaterial(const std::weak_ptr<Material>& material);
      void SetVertexShader(RenderResource&& vertexShader);
      void SetVertexShader(const RenderResource& vertexShader);
      void SetGeometryShader(RenderResource&& geometryShader);
      void SetGeometryShader(const RenderResource& geometryShader);
      void SetTessEvalShader(RenderResource&& tessEvalShader);
      void SetTessEvalShader(const RenderResource& tessEvalShader);
      void SetTessControlShader(RenderResource&& tessControlShader);
      void SetTessControlShader(const RenderResource& tessControlShader);
      void SetFragmentShader(RenderResource&& fragmentShader);
      void SetFragmentShader(const RenderResource& fragmentShader);

      void SetTransform(glm::mat4 transform);
      glm::mat4 Transform(glm::mat4 transform);

      void SetPosition(glm::vec3 pos);
      void Move(glm::vec3 delta);

      glm::mat4 GetTransform(glm::mat4 p = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }) const;
      glm::vec3 GetPosition(glm::vec3 p = { 0, 0, 0 }) const;

      void SetUuid(const lpe::utils::Uuid& uuid);
      lpe::utils::Uuid GetUuid() const;
    };

    class RenderObject
    {
    private:
      std::map<lpe::utils::Uuid, std::vector<std::weak_ptr<RenderTarget>>> parts;

      glm::vec3 position;
      glm::mat4 matrix;
    public:
      RenderObject() = default;
      RenderObject(const RenderObject& other);
      RenderObject(RenderObject&& other) noexcept;
      RenderObject& operator=(const RenderObject& other) = default;
      RenderObject& operator=(RenderObject&& other) noexcept;
      ~RenderObject();

      void SetTransform(glm::mat4 transform);
      glm::mat4 Transform(glm::mat4 transform);

      void SetPosition(glm::vec3 pos);
      void Move(glm::vec3 delta);

      void AddTarget(const std::weak_ptr<RenderTarget>& target);
      void AddTarget(std::weak_ptr<RenderTarget>&& target);

      void Render(vk::CommandBuffer buffer);
    };
  }
}
