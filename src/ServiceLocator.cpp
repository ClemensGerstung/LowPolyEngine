#include "ServiceLocator.h"

lpe::Locator<lpe::utils::IResourceManager> lpe::ServiceLocator::ResourceManager = {};
lpe::Locator<lpe::utils::log::ILogManager> lpe::ServiceLocator::LogManager = {};
lpe::Locator<lpe::rendering::IRenderManager> lpe::ServiceLocator::RenderManager = {};