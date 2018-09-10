#pragma once

#include "Resource.h"
#include <glm/glm.hpp>
#include <map>
#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace render
  {
    class Texture
    {
    private:
      glm::vec4 color;
      std::weak_ptr<utils::Resource> image;
    public:
      Texture() = default;
      Texture(const Texture& other);
      Texture(Texture&& other) noexcept;
      Texture& operator=(const Texture& other) = default;
      Texture& operator=(Texture&& other) noexcept;
      ~Texture();

      void LoadImage(const utils::Uuid& uuid);
      void LoadImage(const char* fileName);
      void SetImage(std::weak_ptr<utils::Resource>&& image);
      void SetImage(const std::weak_ptr<utils::Resource>& image);

      void SetColor(glm::vec4 color);

      void LinkVkImage(/*TODO: std::weak_ptr<Image> buffer*/);
    };

    class Material
    {
    private:
      std::weak_ptr<Texture> albedo;
      std::weak_ptr<Texture> normal;

      // +x, -x, +y, -y, +z, -z
      std::array<std::weak_ptr<Texture>, 6> environment;
    public:
      Material() = default;
      Material(const Material& other);
      Material(Material&& other) noexcept;
      Material& operator=(const Material& other);
      Material& operator=(Material&& other) noexcept;
      ~Material();

      void SetAlbedo(std::weak_ptr<Texture>&& albedo);
      void SetAlbedo(const std::weak_ptr<Texture>& albedo);
      void SetNormal(std::weak_ptr<Texture>&& normal);
      void SetNormal(const std::weak_ptr<Texture>& normal);
      void SetEnvironmentXP(std::weak_ptr<Texture>&& xp);
      void SetEnvironmentXN(const std::weak_ptr<Texture>& xn);
      void SetEnvironmentYP(std::weak_ptr<Texture>&& yp);
      void SetEnvironmentYN(const std::weak_ptr<Texture>& yn);
      void SetEnvironmentZP(std::weak_ptr<Texture>&& zp);
      void SetEnvironmentZN(const std::weak_ptr<Texture>& zn);
    };

    class RenderTarget
    {
    private:
      using RenderResource = std::weak_ptr<utils::Resource>;

      utils::Uuid uuid;
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

      void LinkVkBuffer(/*TODO: std::weak_ptr<Buffer> buffer*/);

      void SetTransform(glm::mat4 transform);
      glm::mat4 Transform(glm::mat4 transform);

      void SetPosition(glm::vec3 pos);
      void Move(glm::vec3 delta);

      glm::mat4 GetTransform(glm::mat4 p = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }) const;
      glm::vec3 GetPosition(glm::vec3 p = { 0, 0, 0 }) const;

      void SetUuid(const utils::Uuid& uuid);
      utils::Uuid GetUuid() const;
    };

    class RenderObject
    {
    private:
      std::map<utils::Uuid, std::vector<std::weak_ptr<RenderTarget>>> parts;

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
