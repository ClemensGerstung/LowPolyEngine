#pragma once

#include "Resource.h"
#include <glm/glm.hpp>

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
      Texture& operator=(const Texture& other);
      Texture& operator=(Texture&& other) noexcept;
      ~Texture() = default;

      void LoadImage(const utils::Uuid& uuid);
      void LoadImage(const char* fileName);
      void SetImage(std::weak_ptr<utils::Resource>&& image);
      void SetImage(const std::weak_ptr<utils::Resource>& image);
    };

    class Material
    {
    private:
      std::weak_ptr<Texture> albedo;
      std::weak_ptr<Texture> normal;
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

      RenderResource mesh;
      std::weak_ptr<Material> material;
      RenderResource vertShader;
      RenderResource geomShader;
      RenderResource tessEvalShader;
      RenderResource tessContShader;
      RenderResource fragShader;
    public:
      RenderTarget() = default;
      RenderTarget(const RenderTarget& other);
      RenderTarget(RenderTarget&& other) noexcept;
      RenderTarget& operator=(const RenderTarget& other);
      RenderTarget& operator=(RenderTarget&& other) noexcept;
      ~RenderTarget();

      void SetMesh(RenderResource&& mesh);
      void SetMesh(const RenderResource& mesh);
    };

    class RenderObject
    {
    };
  }
}
