#ifndef LPE_H
#define LPE_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

#include "stdafx.h"

BEGIN_LPE

class Settings
{
  

public:
  explicit Settings() = default;

  static Settings& GetDefault()
  {
    static Settings settings = {};
    return settings;
  }

  bool EnableValidationLayer = true;
};

namespace helper
{
  template<typename TType>
  class Deleter
  {
  public:
    void operator()(TType* instance) const {}
  };
}

END_LPE

#endif