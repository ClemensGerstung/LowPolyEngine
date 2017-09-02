#ifndef IMAGE_H
#define IMAGE_H
#include <vulkan/vulkan.hpp>

#include "Base.h"

namespace lpe
{
	class ImageView : public Base
	{
		vk::Image image;
		vk::DeviceMemory imageMemory;
		vk::ImageView imageView;

		bool destroyImage;

	public:
		ImageView(vk::PhysicalDevice physicalDevice, const vk::Device& device, bool destroyImage = false);

		~ImageView();

		void Create(vk::Image src, vk::Format format, vk::ImageAspectFlags flags);
		void Create(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::ImageAspectFlags flags);

		vk::ImageView GetImageView() const;
		vk::Image GetImage() const;

		explicit operator vk::ImageView() const;
		explicit operator vk::Image() const;
	};
}

#endif