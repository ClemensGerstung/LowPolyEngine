#include "../include/Commands.h"
#include "../include/RenderPass.h"


lpe::Commands::Commands(const Commands& other)
{
  this->device.reset(other.device.get());
  this->graphicsQueue.reset(other.graphicsQueue.get());

  this->physicalDevice = other.physicalDevice;
  this->commandPool = other.commandPool;
  this->commandBuffers = { other.commandBuffers };
}

lpe::Commands::Commands(Commands&& other) noexcept
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

lpe::Commands& lpe::Commands::operator=(Commands&& other) noexcept
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

lpe::Commands::Commands(vk::PhysicalDevice physicalDevice,
                        vk::Device* device,
                        vk::Queue* graphicsQueue,
                        uint32_t graphicsFamilyIndex)
  : physicalDevice(physicalDevice)
{
  this->device.reset(device);
  this->graphicsQueue.reset(graphicsQueue);

  vk::CommandPoolCreateInfo createInfo = { vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicsFamilyIndex };

  auto result = this->device->createCommandPool(&createInfo,
                                                nullptr,
                                                &commandPool);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create graphics CommandPool!");
}

lpe::Commands::~Commands()
{
  if (graphicsQueue)
  {
    graphicsQueue.release();
  }

  if (device)
  {
    if (!commandBuffers.empty())
    {
      device->freeCommandBuffers(commandPool,
                                 static_cast<uint32_t>(commandBuffers.size()),
                                 commandBuffers.data());
    }

    if (commandPool)
    {
      device->destroyCommandPool(commandPool,
                                 nullptr);
    }

    device.release();
  }
}

void lpe::Commands::ResetCommandBuffers()
{
  if (!commandBuffers.empty())
  {
    for (const auto& cmdBuffer : commandBuffers)
    {
      cmdBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    }
  }
}

void lpe::Commands::RecordCommandBuffers(const std::vector<vk::Framebuffer>& framebuffers,
                                         vk::Extent2D extent,
                                         RenderPass& renderPass,
                                         const std::vector<lpe::Pipeline>& pipelines,
                                         Renderer& renderer)
{
  commandBuffers.resize(framebuffers.size());

  vk::CommandBufferAllocateInfo allocInfo = {
    commandPool,
    vk::CommandBufferLevel::ePrimary,
    (uint32_t)commandBuffers.size()
  };

  auto result = device->allocateCommandBuffers(&allocInfo,
                                               commandBuffers.data());
  helper::ThrowIfNotSuccess(result,
                            "Failed to allocate command buffers!");

  std::array<float, 4> color = { { 0, 0, 0, 1 } };

  std::array<vk::ClearValue, 2> clearValues = {};
  clearValues[0].color = color;
  clearValues[1].depthStencil = vk::ClearDepthStencilValue(1,
                                                           0);

  for (size_t i = 0; i < commandBuffers.size(); i++)
  {
    vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eSimultaneousUse };

    result = commandBuffers[i].begin(&beginInfo);
    helper::ThrowIfNotSuccess(result,
                              "Failed to begin CommandBuffer!");

    vk::RenderPassBeginInfo renderPassInfo = {
      renderPass,
      framebuffers[i],
      { { 0, 0 }, extent },
      (uint32_t)clearValues.size(),
      clearValues.data()
    };
    commandBuffers[i].beginRenderPass(&renderPassInfo,
                                      vk::SubpassContents::eInline);

    if (!renderer.Empty())
    {
      vk::Viewport viewport = { 0, 0, (float)extent.width, (float)extent.height, 0.0, 1.0f };
      commandBuffers[i].setViewport(0,
                                    1,
                                    &viewport);

      vk::Rect2D scissor = { { 0, 0 }, extent };
      commandBuffers[i].setScissor(0,
                                   1,
                                   &scissor);


      //pipeline.UpdateDescriptorSets(ubo.GetDescriptors());

      for (const auto& pipeline : pipelines)
      {
        vk::DescriptorSet set = pipeline.GetDescriptorSet();
        uint32_t prio = pipeline.GetPrio();

        commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             pipeline.GetPipelineLayout(),
                                             0,
                                             1,
                                             &set,
                                             0,
                                             nullptr);

        commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics,
                                       pipeline.GetPipeline());

        renderer.Record(prio, commandBuffers[i]);
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

  vk::FenceCreateInfo fenceCreateInfo = {};
  vk::Fence fence;

  auto result = device->createFence(&fenceCreateInfo,
                                    nullptr,
                                    &fence);
  helper::ThrowIfNotSuccess(result,
                            "Failed to create Fence");

  graphicsQueue->submit(1,
                        &submitInfo,
                        fence);

  result = device->waitForFences(1,
                                 &fence,
                                 VK_TRUE,
                                 std::numeric_limits<uint64_t>::max());
  helper::ThrowIfNotSuccess(result,
                            "Failed to wait for Fences");

  device->destroyFence(fence);

  device->freeCommandBuffers(commandPool,
                             1,
                             &commandBuffer);
}

lpe::ImageView lpe::Commands::CreateDepthImage(vk::Extent2D extent,
                                               vk::Format depthFormat) const
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

  image.TransitionImageLayout(cmdBuffer,
                              depthFormat,
                              vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eDepthStencilAttachmentOptimal);

  EndSingleTimeCommands(cmdBuffer);

  return image;
}
