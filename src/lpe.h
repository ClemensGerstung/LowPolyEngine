#pragma once

namespace lpe
{
  struct Settings
  {
    // more to come

    static Settings Get()
    {
      static Settings settings;

      return settings;
    }
  };
}