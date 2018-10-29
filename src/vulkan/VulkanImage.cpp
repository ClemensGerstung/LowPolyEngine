#include "VulkanImage.hpp"

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetFormat(vk::Format format)
{
  VulkanImage::format = format;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetLevel(uint32_t level)
{
  VulkanImage::level = level;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetLayers(uint32_t layers)
{
  VulkanImage::layers = layers;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetSamples(vk::SampleCountFlagBits samples)
{
  VulkanImage::samples = samples;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetTiling(vk::ImageTiling tiling)
{
  VulkanImage::tiling = tiling;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetUsage(const vk::ImageUsageFlags& usage)
{
  VulkanImage::usage = usage;
  return *this;
}

lpe::rendering::vulkan::VulkanImage& lpe::rendering::vulkan::VulkanImage::SetType(vk::ImageType type)
{
  VulkanImage::type = type;
  return *this;
}
