#include "../include/Commands.h"
#include "../include/ModelsRenderer.h"


lpe::Commands::Commands(const Commands& other)
{
  this->device.reset(other.device.get());
  this->graphicsQueue.reset(other.graphicsQueue.get());

  this->physicalDevice = other.physicalDevice;
  this->commandPool = other.commandPool;
  this->commandBuffers = { other.commandBuffers };
}

lpe::Commands::Commands(Commands&& other)
{
  this->device.reset(other.device.get());
  this->graphicsQueue.reset(other.graphicsQueue.get());

  other.device.release();
  other.graphicsQueue.release();

  this->physicalDevice = other.physicalDevice;
  this->commandPool = other.commandPool;
  this->commandBuffers = std::move(other.commandBuffers);
}

lpe::Commands& lpe::Commands::operator=(const Commands& other)
{
  this->device.reset(other.device.get());
  this->graphicsQueue.reset(other.graphicsQueue.get());

  this->physicalDevice = other.physicalDevice;
  this->commandPool = other.commandPool;
  this->commandBuffers = { other.commandBuffers };

  return *this;
}

lpe::Commands& lpe::Commands::operator=(Commands&& other)
{
  this->device.reset(other.device.get());
  this->graphicsQueue.reset(other.graphicsQueue.get());

  other.device.release();
  other.graphicsQueue.release();

  this->physicalDevice = other.physicalDevice;
  this->commandPool = other.commandPool;
  this->commandBuffers = std::move(other.commandBuffers);

  return *this;
}

lpe::Commands::Commands(vk::PhysicalDevice physicalDevice, vk::Device* device, vk::Queue* graphicsQueue, uint32_t graphicsFamilyIndex)
  : physicalDevice(physicalDevice)
{
  this->device.reset(device);
  this->graphicsQueue.reset(graphicsQueue);

  vk::CommandPoolCreateInfo createInfo = { vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicsFamilyIndex };

  auto result = this->device->createCommandPool(&createInfo, nullptr, &commandPool);
  helper::ThrowIfNotSuccess(result, "failed to create graphics command pool!");
}

lpe::Commands::~Commands()
{
  if(graphicsQueue)
  {
    graphicsQueue.release();
  }

  if(device)
  {
    if (commandBuffers.size() > 0)
    {
      device->freeCommandBuffers(commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }

    if(commandPool)
    {
      device->destroyCommandPool(commandPool, nullptr);
    }

    device.release();
  }
}

void lpe::Commands::ResetCommandBuffers()
{
  if (commandBuffers.size() > 0)
  {
    for (const auto& cmdBuffer : commandBuffers)
    {
      cmdBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    }
  }
}

void lpe::Commands::CreateCommandBuffers(const std::vector<vk::Framebuffer>& framebuffers,
                                         vk::Extent2D extent,
                                         size_t dynamicAlignment,
                                         lpe::Pipeline* pipeline,
                                         ModelsRenderer* renderer)
{
  commandBuffers.resize(framebuffers.size());

  vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, (uint32_t)commandBuffers.size() };

  auto result = device->allocateCommandBuffers(&allocInfo, commandBuffers.data());
  helper::ThrowIfNotSuccess(result, "failed to allocate command buffers!");

  std::array<float, 4> color = { { 0, 0, 0, 1 } };

  std::array<vk::ClearValue, 2> clearValues = {};
  clearValues[0].color = color;
  clearValues[1].depthStencil = vk::ClearDepthStencilValue(1, 0);

  for (size_t i = 0; i < commandBuffers.size(); i++)
  {
    vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eSimultaneousUse };

    result = commandBuffers[i].begin(&beginInfo);
    helper::ThrowIfNotSuccess(result, "failed to begin commandbuffer!");

    vk::RenderPassBeginInfo renderPassInfo = { pipeline->GetRenderPass(), framebuffers[i], { { 0, 0 }, extent }, (uint32_t)clearValues.size(), clearValues.data() };
    commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

    commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline->GetPipelineRef());

    if (*renderer->GetVertexBuffer() && *renderer->GetIndexBuffer())
    {
      if (!*pipeline->GetDescriptorSetRef())
      {
        pipeline->CreateDescriptorSet();
      }

      vk::Viewport viewport = { 0, 0, (float)extent.width, (float)extent.height, 0.0, 1.0f };
      commandBuffers[i].setViewport(0, 1, &viewport);

      vk::Rect2D scissor = { {0, 0}, extent };
      commandBuffers[i].setScissor(0, 1, &scissor);

      VkDeviceSize offsets[1] = {0};
      commandBuffers[i].bindVertexBuffers(0, 1, renderer->GetVertexBuffer(), offsets);
      commandBuffers[i].bindIndexBuffer(*renderer->GetIndexBuffer(), 0, vk::IndexType::eUint32);

      for (uint32_t j = 0; j < renderer->GetCount(); j++)
      {
        // One dynamic offset per dynamic descriptor to offset into the ubo containing all model matrices
        uint32_t dynamicOffset = j * static_cast<uint32_t>(dynamicAlignment);
        // Bind the descriptor set for rendering a mesh using the dynamic offset
        commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             pipeline->GetPipelineLayout(),
                                             0,
                                             1,
                                             pipeline->GetDescriptorSetRef(),
                                             1,
                                             &dynamicOffset);
        
        //commandBuffers[i].drawIndexedIndirect(*renderer->GetIndexBuffer(), 0, (uint32_t)renderer->GetIndices().size(), 0);
        commandBuffers[i].drawIndexed((uint32_t)renderer->GetIndices().size(), 1, 0, 0, 0);
      }

      
    }

    commandBuffers[i].endRenderPass();
    commandBuffers[i].end();
  }
}

vk::CommandBuffer lpe::Commands::BeginSingleTimeCommands() const
{
  vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, 1 };
  vk::CommandBuffer commandBuffer = device->allocateCommandBuffers(allocInfo)[0];

  vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

  commandBuffer.begin(beginInfo);

  return commandBuffer;
}

vk::CommandBuffer lpe::Commands::operator[](uint32_t index)
{
  return commandBuffers[index];
}

void lpe::Commands::EndSingleTimeCommands(vk::CommandBuffer commandBuffer) const
{
  commandBuffer.end();

  vk::SubmitInfo submitInfo = { 0, nullptr, nullptr, 1, &commandBuffer };

  graphicsQueue->submit(1, &submitInfo, nullptr);
  graphicsQueue->waitIdle();

  device->freeCommandBuffers(commandPool, 1, &commandBuffer);
}

lpe::Buffer lpe::Commands::CreateBuffer(void* data, vk::DeviceSize size) const
{
  Buffer staging = {physicalDevice, device.get(), data, size};

  Buffer actual = { physicalDevice, device.get(), size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal };

  auto commandBuffer = BeginSingleTimeCommands();

  actual.Copy(staging, commandBuffer);

  EndSingleTimeCommands(commandBuffer);

  return actual;
}

lpe::Buffer lpe::Commands::CreateBuffer(vk::DeviceSize size) const
{
  return { physicalDevice, device.get(), size };
}

lpe::ImageView lpe::Commands::CreateDepthImage(vk::Extent2D extent, vk::Format depthFormat) const
{
  ImageView image =
  {
    physicalDevice,
    device.get(),
    extent.width,
    extent.height,
    depthFormat,
    vk::ImageTiling::eOptimal,
    vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc,
    vk::MemoryPropertyFlagBits::eDeviceLocal,
    vk::ImageAspectFlagBits::eDepth
  };

  auto cmdBuffer = BeginSingleTimeCommands();

  image.TransitionImageLayout(cmdBuffer, depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

  EndSingleTimeCommands(cmdBuffer);

  return image;
}
