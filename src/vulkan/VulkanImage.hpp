#ifndef LOWPOLYENGINE_VULKANIMAGE_H
#define LOWPOLYENGINE_VULKANIMAGE_H

#include "VulkanManager.hpp"

namespace lpe
{
namespace rendering
{
namespace vulkan
{

class VulkanImage
{
private:
  std::weak_ptr<VulkanManager> manager;

  vk::Image image;
  vk::ImageView imageView;
  vk::Format format;
  uint32_t level;
  uint32_t layers;
  vk::SampleCountFlagBits samples;
  vk::ImageTiling tiling;
  vk::ImageUsageFlags usage;
  vk::ImageType type;
public:
  VulkanImage();
  ~VulkanImage();

  /*!
   * Creates an imageview for an existing image.
   * Used for the swapchain images
   *
   * @param device
   * @param image
   * @return
   */
  bool Create(VulkanManager* manager, vk::Image image);

  VulkanImage& SetFormat(vk::Format format);

  VulkanImage& SetLevel(uint32_t level);

  VulkanImage& SetLayers(uint32_t layers);

  VulkanImage& SetSamples(vk::SampleCountFlagBits samples);

  VulkanImage& SetTiling(vk::ImageTiling tiling);

  VulkanImage& SetUsage(const vk::ImageUsageFlags& usage);

  VulkanImage& SetType(vk::ImageType type);
};

}
}
}


#endif //LOWPOLYENGINE_VULKANIMAGE_H
