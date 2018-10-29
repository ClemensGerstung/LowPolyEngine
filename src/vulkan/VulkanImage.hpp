#ifndef LOWPOLYENGINE_VULKANIMAGE_H
#define LOWPOLYENGINE_VULKANIMAGE_H

#include <vulkan/vulkan.hpp>
#include "../LogManager.h"

namespace lpe
{
namespace rendering
{
namespace vulkan
{

class VulkanManager;

class VulkanImage
{
private:
  std::weak_ptr<VulkanManager> manager;
  std::weak_ptr<lpe::utils::log::ILogManager> logger;

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
public:
  VulkanImage();

  ~VulkanImage() = default;

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
};

}
}
}


#endif //LOWPOLYENGINE_VULKANIMAGE_H
