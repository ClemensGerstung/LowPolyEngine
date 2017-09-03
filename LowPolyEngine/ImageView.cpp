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
                            vk::ImageAspectFlags flags,
                            bool createVulkanImageView)
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

	if (createVulkanImageView) 
	{
		Create(image, format, flags);
	}
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

void lpe::ImageView::TransitionImageLayout(lpe::Commands& commands, const vk::Queue& graphicsQueue, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	auto commandBuffer = commands.BeginSingleTimeCommands();

	vk::ImageSubresourceRange range = { {}, 0, 1, 0, 1 };
	vk::AccessFlags srcAccessMask;
	vk::AccessFlags dstAccessMask;
	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if(newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		range.aspectMask = vk::ImageAspectFlagBits::eDepth;
		if(format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint)
		{
			range.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	else
	{
		range.aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		srcAccessMask = {};
		dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		srcAccessMask = {};
		dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}


	vk::ImageMemoryBarrier barrier = { srcAccessMask, dstAccessMask, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, range };

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1, &barrier);

	commands.EndSingleTimeCommands(commandBuffer, graphicsQueue);
}
