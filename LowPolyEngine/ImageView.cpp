#include "ImageView.h"

lpe::ImageView::ImageView(const ImageView& other)
{
  this->device.reset(other.device.get());
  this->image = other.image;
  this->imageView = other.imageView;
}

lpe::ImageView::ImageView(ImageView&& other)
{
  this->device.reset(other.device.get());
  other.device.release();
  this->image = other.image;
  this->imageView = other.imageView;
}

lpe::ImageView& lpe::ImageView::operator=(const ImageView& other)
{
  this->device.reset(other.device.get());
  this->image = other.image;
  this->imageView = other.imageView;

  return *this;
}

lpe::ImageView& lpe::ImageView::operator=(ImageView&& other)
{
  this->device.reset(other.device.get());
  other.device.release();
  this->image = other.image;
  this->imageView = other.imageView;

  return *this;
}

lpe::ImageView::ImageView(vk::Device* device,
                          const vk::Image& image,
                          vk::Format format,
                          vk::ImageAspectFlags flags)
{
  this->device.reset(device);

  vk::ImageViewCreateInfo createInfo = { {}, image, vk::ImageViewType::e2D, format,{},{ flags, 0, 1, 0, 1 } };
  auto result = device->createImageView(&createInfo, nullptr, &imageView);
  helper::ThrowIfNotSuccess(result, "failed to create texture image view!");
}

lpe::ImageView::~ImageView()
{
  if(device)
  {
    if(imageView)
    {
      device->destroyImageView(imageView, nullptr);
    }

    if(image)
    {
      device->destroyImage(image, nullptr);
    }

    device.release();
  }
}
