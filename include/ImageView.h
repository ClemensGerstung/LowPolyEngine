#ifndef ImageView_H
#define ImageView_H
#include "stdafx.h"

BEGIN_LPE

class ImageView
{
private:
  std::unique_ptr<vk::Device> device;

  vk::Image image;
  vk::ImageView imageView;
  vk::DeviceMemory memory;

public:
  ImageView() = default;
  ImageView(const ImageView& other);
  ImageView(ImageView&& other);
  ImageView& operator=(const ImageView& other);
  ImageView& operator=(ImageView&& other);

  ImageView(vk::Device* device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags flags);
  ImageView(vk::PhysicalDevice physicalDevice, vk::Device* device, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::ImageAspectFlags flags);

  ~ImageView();

  void TransitionImageLayout(vk::CommandBuffer commandBuffer, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

  vk::ImageView GetImageView() const;
};

END_LPE

#endif
