#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>
#include <vulkan/vulkan.hpp>
#include "Base.h"
#include "Commands.h"
#include "ImageView.h"

namespace lpe
{
	class Texture : public Base
	{
		lpe::ImageView image;
		vk::Sampler sampler;
	public:
		Texture() = default;
		Texture(vk::PhysicalDevice physicalDevice, const vk::Device& device);
		~Texture();

		void Create(lpe::Commands& commands, const vk::Queue graphicsQueue, std::string textureFileName);


		vk::Sampler GetSampler() const;
		vk::Image GetImage() const;
		vk::ImageView GetImageView() const;
	};
}

#endif
