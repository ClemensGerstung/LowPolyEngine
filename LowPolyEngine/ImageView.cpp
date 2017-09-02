#include "ImageView.h"

lpe::ImageView::ImageView(vk::PhysicalDevice physicalDevice, const vk::Device& device, bool destroyImage)
	: Base(physicalDevice, device)
{
	this->destroyImage = destroyImage;
}

lpe::ImageView::~ImageView()
{
	device.destroyImageView(imageView);
	if (destroyImage) 
	{
		device.destroyImage(image);
		device.freeMemory(imageMemory);
	}
}

void lpe::ImageView::Create(vk::Image src, vk::Format format, vk::ImageAspectFlags flags)
{
	vk::ImageViewCreateInfo createInfo = {{}, src, vk::ImageViewType::e2D, format, {}, {flags, 0, 1, 0, 1}};

	auto result = device.createImageView(&createInfo, nullptr, &imageView);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create texture image view! " + vk::to_string(result));
	}
}

void lpe::ImageView::Create(uint32_t width,
                            uint32_t height,
                            vk::Format format,
                            vk::ImageTiling tiling,
                            vk::ImageUsageFlags usage,
                            vk::MemoryPropertyFlags properties,
							vk::ImageAspectFlags flags)
{
	vk::ImageCreateInfo createInfo = 
	{
		{}, 
		vk::ImageType::e2D, 
		format, 
		{
			width, 
			height, 
			1
		}, 
		1, 
		1, 
		vk::SampleCountFlagBits::e1, 
		tiling,
		usage,
		vk::SharingMode::eExclusive, 
		0, 
		nullptr,
		vk::ImageLayout::eUndefined
	};

	auto result = device.createImage(&createInfo, nullptr, &image);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create image! (" + vk::to_string(result) + ")");
	}

	vk::MemoryRequirements requirements = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo = { requirements.size, FindMemoryTypeIndex(requirements.memoryTypeBits, properties) };

	result = device.allocateMemory(&allocInfo, nullptr, &imageMemory);

	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to allocate image memory! (" + vk::to_string(result) + ")");
	}

	device.bindImageMemory(image, imageMemory, 0);

	Create(image, format, flags);
}

vk::ImageView lpe::ImageView::GetImageView() const
{
	return imageView;
}

vk::Image lpe::ImageView::GetImage() const
{
	return image;
}

lpe::ImageView::operator vk::ImageView() const
{
	return imageView;
}

lpe::ImageView::operator vk::Image() const
{
	return image;
}
