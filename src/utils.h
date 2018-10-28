#pragma once

#include <memory>

namespace lpe
{
  namespace utils
  {
    template<typename TType>
    class Deleter
    {
    public:
      void operator()(TType* instance) const {}
    };

    template<typename TType>
    using SimplePointer = std::unique_ptr<TType, Deleter<TType>>;

  }
}