#pragma once

#include "ServiceLocator.h"
#include "Uuid.h"
#include "Event.h"
#include "MersenneTwister.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "RenderManager.h"

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