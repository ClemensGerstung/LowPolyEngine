#include "Commands.h"


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

  vk::CommandPoolCreateInfo createInfo = { {}, graphicsFamilyIndex };

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

vk::CommandBuffer lpe::Commands::BeginSingleTimeCommands() const
{
  vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, 1 };
  vk::CommandBuffer commandBuffer = device->allocateCommandBuffers(allocInfo)[0];

  vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

  commandBuffer.begin(beginInfo);

  return commandBuffer;
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
