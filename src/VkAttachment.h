#pragma once

#include <vulkan/vulkan.hpp>

namespace lpe
{
  namespace render
  {
    struct Attachment
    {
      vk::ImageView ImageView;
      uint32_t Index;
      vk::ImageLayout Layout;
      vk::AttachmentDescriptionFlags Flags;
      vk::Format Format;
      vk::SampleCountFlagBits Samples;
      vk::AttachmentLoadOp LoadOp;
      vk::AttachmentStoreOp StoreOp;
      vk::AttachmentLoadOp StencilLoadOp;
      vk::AttachmentStoreOp StencilStoreOp;
      vk::ImageLayout InitialLayout;
      vk::ImageLayout FinalLayout;

      Attachment() = default;
      Attachment(const Attachment& other) = default;
      Attachment(Attachment&& other) noexcept = default;
      Attachment& operator=(const Attachment& other) = default;
      Attachment& operator=(Attachment&& other) noexcept = default;

      Attachment(const vk::ImageView& imageView,
        uint32_t index,
        vk::ImageLayout layout,
        const vk::AttachmentDescriptionFlags& flags,
        vk::Format format,
        vk::SampleCountFlagBits samples,
        vk::AttachmentLoadOp loadOp,
        vk::AttachmentStoreOp storeOp,
        vk::AttachmentLoadOp stencilLoadOp,
        vk::AttachmentStoreOp stencilStoreOp,
        vk::ImageLayout initialLayout,
        vk::ImageLayout finalLayout);

      ~Attachment() = default;

      vk::AttachmentReference GetAttachmentReference() const;
      vk::AttachmentDescription GetAttachmentDescription() const;

      operator bool() const;
      bool operator!() const;
    };
  }
}