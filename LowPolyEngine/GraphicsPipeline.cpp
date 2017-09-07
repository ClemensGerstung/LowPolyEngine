#include "GraphicsPipeline.h"
#include "stdafx.h"
#include "Vertex.h"
#include "UniformBufferObject.h"

vk::Format lpe::GraphicsPipeline::FindDepthFormat() const
{
	vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

	for (vk::Format format : {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint})
	{
		auto props = physicalDevice.getFormatProperties(format);

		if ((props.linearTilingFeatures & features) == features)
		{
			return format;
		}

		if ((props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

vk::ShaderModule lpe::GraphicsPipeline::CreateShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo createInfo = { {}, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };

	vk::ShaderModule shaderModule;

	auto result = device.createShaderModule(&createInfo, nullptr, &shaderModule);

	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create shader module! (" + vk::to_string(result) + ")");
	}

	return shaderModule;
}

void lpe::GraphicsPipeline::CreateRenderPass(vk::Format swapChainImageFormat)
{
	vk::AttachmentDescription colorAttachment = 
	{
		{},
		swapChainImageFormat,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare, 
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR
	};
	vk::AttachmentDescription depthAttachment = 
	{
		{}, 
		FindDepthFormat(),
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eDontCare,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthStencilAttachmentOptimal
	};

	vk::AttachmentReference colorAttachmentRef = 
	{
		0, 
		vk::ImageLayout::eColorAttachmentOptimal
	};

	vk::AttachmentReference depthAttachmentRef = 
	{
		1,
		vk::ImageLayout::eDepthStencilAttachmentOptimal
	};

	vk::SubpassDescription subpass = 
	{
		{},
		vk::PipelineBindPoint::eGraphics,
		0, 
		nullptr,
		1,
		&colorAttachmentRef,
		nullptr,
		&depthAttachmentRef, 
		0,
		nullptr
	};

	vk::SubpassDependency dependency = 
	{
		VK_SUBPASS_EXTERNAL,
		0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		{},
		vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
	};

	std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
	
	vk::RenderPassCreateInfo renderPassInfo =
	{
		{},
		(uint32_t)attachments.size(), 
		attachments.data(),
		1,
		&subpass,
		1,
		&dependency
	};

	auto result = device.createRenderPass(&renderPassInfo, nullptr, &renderPass);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create render pass! (" + vk::to_string(result) + ")");
	}
}

void lpe::GraphicsPipeline::CreateDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding = { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex };
	vk::DescriptorSetLayoutBinding samplerLayoutBinding = { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment };

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	vk::DescriptorSetLayoutCreateInfo layoutInfo = { {}, (uint32_t)bindings.size(), bindings.data() };

	auto result = device.createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout);

	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create descriptor set layout! (" + vk::to_string(result) + ")");
	}
}

void lpe::GraphicsPipeline::CreateGraphicsPipeline(vk::Extent2D swapChainExtent)
{
	auto vertexShaderCode = lpe::helper::ReadSPIRVFile("shaders/vertex.spv");
	auto fragmentShaderCode = lpe::helper::ReadSPIRVFile("shaders/fragment.spv");

	auto vertexShaderModule = CreateShaderModule(vertexShaderCode);
	auto fragmentShaderModule = CreateShaderModule(fragmentShaderCode);

	vk::PipelineShaderStageCreateInfo vertexShaderStageInfo = { {}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main" };
	vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo = { {}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main" };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = { vertexShaderStageInfo, fragmentShaderStageInfo };

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = { {}, 1, &bindingDescription, (uint32_t)attributeDescriptions.size(), attributeDescriptions.data() };

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {}, vk::PrimitiveTopology::eTriangleList, VK_FALSE };

	vk::Viewport viewport = { 0.f, 0.f, (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };

	vk::Rect2D scissor = { {0, 0}, swapChainExtent };

	vk::PipelineViewportStateCreateInfo viewportState = { {}, 1, &viewport, 1, &scissor };

	vk::PipelineRasterizationStateCreateInfo rasterizer = { {}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, VK_FALSE, 0, 0, 0, 1 };

	vk::PipelineMultisampleStateCreateInfo multisampling = { {}, vk::SampleCountFlagBits::e1, VK_FALSE };

	vk::PipelineDepthStencilStateCreateInfo depthStencil = { {}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess, VK_FALSE, VK_FALSE };

	vk::PipelineColorBlendAttachmentState colorBlendAttachment = { VK_FALSE };
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

	vk::PipelineColorBlendStateCreateInfo colorBlending = { {}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment };

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = { {}, 1, &descriptorSetLayout };

	auto result = device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create pipeline layout! (" + vk::to_string(result) + ")");
	}

	vk::GraphicsPipelineCreateInfo pipelineInfo = { {}, (uint32_t)shaderStages.size(), shaderStages.data(), &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, &depthStencil, &colorBlending, nullptr, pipelineLayout, renderPass};

	graphicsPipeline = device.createGraphicsPipeline(nullptr, pipelineInfo);

	device.destroyShaderModule(vertexShaderModule);
	device.destroyShaderModule(fragmentShaderModule);
}

void lpe::GraphicsPipeline::CreateDescriptorPool(const vk::Device& device)
{
	std::array<vk::DescriptorPoolSize, 2> poolSizes;
	poolSizes[0] = { vk::DescriptorType::eUniformBuffer, 1 };
	poolSizes[1] = { vk::DescriptorType::eCombinedImageSampler, 1 };

	vk::DescriptorPoolCreateInfo poolInfo = { {}, 1, (uint32_t)poolSizes.size(), poolSizes.data() };

	auto result = device.createDescriptorPool(&poolInfo, nullptr, &descriptorPool);

	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create descriptor pool! (" + vk::to_string(result) + ")");
	}
}

void lpe::GraphicsPipeline::CreateDescriptorSet(vk::Device* device,
                                                vk::Buffer* uniformBuffer,
                                                lpe::Texture* texture)
{
	std::array<vk::DescriptorSetLayout, 1> layouts = { descriptorSetLayout };

	vk::DescriptorSetAllocateInfo allocInfo = { descriptorPool, (uint32_t)layouts.size(), layouts.data() };

	auto result = device->allocateDescriptorSets(&allocInfo, &descriptorSet);
	if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to allocate descriptor set! (" + vk::to_string(result) + ")");
	}

	vk::DescriptorBufferInfo bufferInfo = { *uniformBuffer, 0, sizeof(UniformBufferObject) };

	vk::DescriptorImageInfo imageInfo = { *texture->GetSamplerRef(), *texture->GetImageViewRef(), vk::ImageLayout::eShaderReadOnlyOptimal };

	std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};
	descriptorWrites[0] = vk::WriteDescriptorSet(descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo);
	descriptorWrites[1] = vk::WriteDescriptorSet(descriptorSet, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo);

	device->updateDescriptorSets((uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

lpe::GraphicsPipeline::~GraphicsPipeline()
{
	device.destroyPipeline(graphicsPipeline);
	device.destroyPipelineLayout(pipelineLayout);
	device.destroyRenderPass(renderPass);
}

void lpe::GraphicsPipeline::Create(vk::PhysicalDevice physicalDevice,
                                   const vk::Device& device,
                                   vk::Format swapChainImageFormat,
                                   vk::Extent2D swapChainExtent)
{
	this->physicalDevice = physicalDevice;
	this->device = device;

	CreateRenderPass(swapChainImageFormat);

	CreateDescriptorSetLayout();

	CreateGraphicsPipeline(swapChainExtent);
}

void lpe::GraphicsPipeline::Finalize(vk::Device* device, vk::Buffer* uniformBuffer, lpe::Texture* texture)
{
	CreateDescriptorPool(*device);

	CreateDescriptorSet(device, uniformBuffer, texture);
}

vk::RenderPass lpe::GraphicsPipeline::GetRenderPass() const
{
	return renderPass;
}

vk::DescriptorSetLayout lpe::GraphicsPipeline::GetDescriptorSetLayout() const
{
	return descriptorSetLayout;
}

vk::PipelineLayout lpe::GraphicsPipeline::GetPipelineLayout() const
{
	return pipelineLayout;
}

vk::Pipeline lpe::GraphicsPipeline::GetGraphicsPipeline() const
{
	return graphicsPipeline;
}

vk::DescriptorSet lpe::GraphicsPipeline::GetDescriptorSet() const
{
	return descriptorSet;
}
