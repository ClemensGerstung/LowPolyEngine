#pragma once
#include <vector>
#include <iostream>
#include <functional>

namespace lpe
{
  class Settings
  {
  private:
    std::vector<const char*> validationLayers;
    std::vector<const char*> deviceExtensions;
  public:
    bool EnableValidationLayers = true;
    std::function<void(const char*)> Output;

    Settings()
    {
      validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
      deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

      Output = [](const char* message) { std::cout << message << std::endl; };
    }

    ~Settings() = default;

    static Settings Default()
    {
      static Settings settings;

      return settings;
    }

    void AddValidationLayer(const char* layerName)
    {
      validationLayers.push_back(layerName);
    }

    std::vector<const char*> GetValidationLayers() const
    {
      return validationLayers;
    }

    void RemoveValidationLayer(const char* layerName)
    {
      validationLayers.erase(std::find(validationLayers.begin(),
                                       validationLayers.end(),
                                       layerName));
    }

    void AddDeviceExtension(const char* name)
    {
      deviceExtensions.push_back(name);
    }

    std::vector<const char*> GetDeviceExtensions() const
    {
      return deviceExtensions;
    }
  };
}
