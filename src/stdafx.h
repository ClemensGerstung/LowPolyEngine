#pragma once
#include <vector>

namespace lpe
{
  constexpr uint32_t LPE_Version = VK_MAKE_VERSION(0, 0, 1);

  class Settings
  {
  private:
    std::vector<const char*> validationLayers;
    std::vector<const char*> deviceExtensions;
  public:
    bool EnableValidationLayers = true;
    
    Settings()
    {
      validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
      deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
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
