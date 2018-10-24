#pragma once

#include "../../src/ServiceLocator.h"
#include "../../src/Uuid.h"
#include "../../src/Event.h"
#include "../../src/MersenneTwister.h"
#include "../../src/Resource.h"
#include "../../src/ResourceManager.h"
#include "../../src/LogManager.h"
#include "../../src/RenderManager.h"
#include "../../src/RenderObject.h"
#include "../../src/VkAttachment.h"
#include "../../src/VkMemoryManagement.h"
#include "../../src/VkRenderPass.h"
#include "../../src/VkTexture.h"

namespace lpe
{
  inline void Initialize()
  {
    utils::ResourceManager resourceManager ={};
    resourceManager.Initialize();
    ServiceLocator::ResourceManager.Provide<>(&resourceManager);

    utils::log::LogManager log = {};
    log.Initialize();
    ServiceLocator::LogManager.Provide<>(&log);


  }
}