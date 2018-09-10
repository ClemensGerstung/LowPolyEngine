#include "RenderPass.h"

vk::AttachmentReference lpe::render::Attachment::GetAttachmentReference() const
{
  return { Index, Layout };
}

vk::AttachmentDescription lpe::render::Attachment::GetAttachmentDescription() const
{
  return { Flags, Format, Samples, LoadOp, StoreOp, StencilLoadOp, StencilStoreOp, InitialLayout, FinalLayout };
}

lpe::render::Attachment::operator bool() const
{
  return Image && Flags;
}

bool lpe::render::Attachment::operator!() const
{
  return !(Image && Flags);
}


