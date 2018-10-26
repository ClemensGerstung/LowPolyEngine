#include "ServiceLocator.h"

lpe::Locator<lpe::utils::IResourceManager> lpe::ServiceLocator::ResourceManager = {};
lpe::Locator<lpe::utils::log::ILogManager> lpe::ServiceLocator::LogManager = {};
lpe::Locator<lpe::render::IRenderManager> lpe::ServiceLocator::RenderManager = {};