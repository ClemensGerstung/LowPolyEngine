#include "Texture.h"
#include "Buffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "ImageView.h"

lpe::Texture::Texture(vk::PhysicalDevice physicalDevice, const vk::Device& device)
	: Base(physicalDevice, device)
{
}

lpe::Texture::~Texture()
{
	if(!created)
		return;

	device.destroySampler(sampler, nullptr);
}

void lpe::Texture::Create(lpe::Commands& commands, const vk::Queue graphicsQueue, std::string textureFileName)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(textureFileName.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image! (" + textureFileName + ")");
	}

	Buffer b = { physicalDevice, device };
	b.CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, b.buffer, b.memory);

	void* data;
	device.mapMemory(b.memory, 0, imageSize, {}, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	device.unmapMemory(b.memory);

	image = { physicalDevice, device };
	image.Create(texWidth, texHeight, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, {}, false);
	image.TransitionImageLayout(commands, graphicsQueue, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

	vk::CommandBuffer commandBuffer = commands.BeginSingleTimeCommands();

	vk::BufferImageCopy region = { 0, 0, 0, { vk::ImageAspectFlagBits::eColor, 0, 0, 1 }, {0, 0, 0}, {(uint32_t)texWidth, (uint32_t)texHeight, 1} };
	commandBuffer.copyBufferToImage(b.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

	commands.EndSingleTimeCommands(commandBuffer, graphicsQueue);

	image.TransitionImageLayout(commands, graphicsQueue, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

	image.Create(image.image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);

	vk::SamplerCreateInfo samplerInfo = 
	{
		{}, 
		vk::Filter::eLinear,
		vk::Filter::eLinear,
		vk::SamplerMipmapMode::eLinear,
		vk::SamplerAddressMode::eMirroredRepeat,
		vk::SamplerAddressMode::eMirroredRepeat,
		vk::SamplerAddressMode::eMirroredRepeat,
		0,
		VK_TRUE,
		16, 
		VK_FALSE,
		vk::CompareOp::eAlways,
		0, 
		0, 
		vk::BorderColor::eIntOpaqueBlack,
		VK_FALSE
	};

	auto result = device.createSampler(&samplerInfo, nullptr, &sampler);

	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create texture sampler! (" + vk::to_string(result) + ")");
	}

	created = true;
}

vk::Sampler lpe::Texture::GetSampler() const
{
	return sampler;
}

vk::Image lpe::Texture::GetImage() const
{
	return image.image;
}

vk::ImageView lpe::Texture::GetImageView() const
{
	return image.imageView;
}

vk::Sampler& lpe::Texture::GetSamplerRef()
{
	return sampler;
}

vk::Image& lpe::Texture::GetImageRef()
{
	return image.image;
}

vk::ImageView& lpe::Texture::GetImageViewRef()
{
	return image.imageView;
}
