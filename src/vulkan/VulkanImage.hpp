#ifndef LOWPOLYENGINE_VULKANIMAGE_H
#define LOWPOLYENGINE_VULKANIMAGE_H

// #define STB_IMAGE_IMPLEMENTATION in lpe.hpp
#include <stb_image.h>
#include <vulkan/vulkan.hpp>
#include "../LogManager.h"
#include "../Resource.h"

namespace lpe
{
namespace rendering
{
namespace vulkan
{
namespace common
{
bool CreateImageView(vk::Device device,
                     vk::Image image,
                     vk::ImageView& view,
                     vk::ImageViewType viewType,
                     vk::Format format,
                     vk::ImageAspectFlags aspectFlags,
                     uint32_t baseMipLevel = 0,
                     uint32_t mipLevels = VK_REMAINING_MIP_LEVELS,
                     uint32_t baseArrayLayer = 0,
                     uint32_t arrayLayers = VK_REMAINING_ARRAY_LAYERS,
                     vk::ComponentSwizzle r = vk::ComponentSwizzle::eR,
                     vk::ComponentSwizzle g = vk::ComponentSwizzle::eG,
                     vk::ComponentSwizzle b = vk::ComponentSwizzle::eB,
                     vk::ComponentSwizzle a = vk::ComponentSwizzle::eA);
}

class VulkanManager;

class VulkanImage
{
private:
  std::weak_ptr<VulkanManager> manager;
  std::weak_ptr<lpe::utils::log::ILogManager> logger;

  vk::Device device;  // hold actual VkDevice handle in case manager ptr gets lost
  vk::Image image;
  vk::ImageView imageView;
  vk::Format format;
  uint32_t mipLevels;
  uint32_t baseMipLevel;
  uint32_t layers;
  uint32_t baseLayer;
  vk::SampleCountFlagBits samples;
  vk::ImageTiling tiling;
  vk::ImageUsageFlags usage;
  vk::ImageType type;
  vk::ImageViewType viewType;
  vk::ImageAspectFlags aspectFlags;
  vk::Extent3D extent;
public:
  VulkanImage();

  ~VulkanImage() = default;

  bool Create(std::shared_ptr<VulkanManager>&& manager);

  /*!
   * Creates an imageview for an existing image.
   * Used for the swapchain images
   *
   * @param manager
   * @param image
   * @return
   */
  bool Create(std::shared_ptr<VulkanManager>&& manager,
              vk::Image image);

  /*!
   * Creates a 2D vk::Image and vk::ImageView from a resource
   *
   * @param manager
   * @param resource
   * @param desiredChannels
   * @return
   */
  bool Create(std::shared_ptr<VulkanManager>&& manager,
              std::weak_ptr<lpe::utils::Resource> resource,
              int desiredChannels = STBI_rgb_alpha);

  void Destroy();



  VulkanImage& SetFormat(vk::Format format);

  VulkanImage& SetLevel(uint32_t level);

  VulkanImage& SetLayers(uint32_t layers);

  VulkanImage& SetSamples(vk::SampleCountFlagBits samples);

  VulkanImage& SetTiling(vk::ImageTiling tiling);

  VulkanImage& SetUsage(const vk::ImageUsageFlags& usage);

  VulkanImage& SetType(vk::ImageType type);

  VulkanImage& SetMipLevels(uint32_t mipLevels);

  VulkanImage& SetBaseMipLevel(uint32_t baseMipLevel);

  VulkanImage& SetBaseLayer(uint32_t baseLayer);

  VulkanImage& SetViewType(vk::ImageViewType viewType);

  VulkanImage& SetAspectFlags(const vk::ImageAspectFlags& aspectFlags);

  VulkanImage& SetWidth(uint32_t width);

  VulkanImage& SetHeight(uint32_t height);

  VulkanImage& SetDepth(uint32_t depth);
};

}
}
}


#endif //LOWPOLYENGINE_VULKANIMAGE_H
