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

public:
  ImageView() = default;
  ImageView(const ImageView& other);
  ImageView(ImageView&& other);
  ImageView& operator=(const ImageView& other);
  ImageView& operator=(ImageView&& other);

  ImageView(vk::Device* device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags flags);

  ~ImageView();
};

END_LPE

#endif
