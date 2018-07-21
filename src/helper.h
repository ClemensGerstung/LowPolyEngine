#pragma once
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include "stdafx.h"

namespace lpe
{
  namespace helper
  {
    VULKAN_HPP_INLINE void ThrowIfNotSuccess(vk::Result result,
                                             std::string message)
    {
#if _DEBUG
      assert(result == vk::Result::eSuccess);
#endif

      if (result != vk::Result::eSuccess)
      {
        Settings::Default().Output((message + " (Result: " + vk::to_string(result) + ")").c_str());
      }
    }

    template <typename CallbackCreateInfo,
              typename Callback,
              typename CallbackFunctionType>
    VULKAN_HPP_INLINE VkResult CreateCallbackEXT(VkInstance instance,
                                                 const CallbackCreateInfo* pCreateInfo,
                                                 const VkAllocationCallbacks* pAllocator,
                                                 Callback* pCallback,
                                                 const char* pCallbackFunctionName)
    {
      auto func = (CallbackFunctionType)vkGetInstanceProcAddr(instance,
                                                              pCallbackFunctionName);
      if (func != nullptr)
      {
        return func(instance,
                    pCreateInfo,
                    pAllocator,
                    pCallback);
      }

      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    template <typename Callback,
              typename CallbackFunctionType>
    VULKAN_HPP_INLINE void DestroyCallbackExt(VkInstance instance,
                                              Callback callback,
                                              const VkAllocationCallbacks* pAllocator,
                                              const char* pCallbackFunctionName)
    {
      auto func = (CallbackFunctionType)vkGetInstanceProcAddr(instance,
                                                              pCallbackFunctionName);
      if (func != nullptr)
      {
        func(instance,
             callback,
             pAllocator);
      }
    }
  
    template<typename TType>
    class Deleter
    {
    public:
      void operator()(TType* instance) const {}
    };

    template<typename TType>
    using Pointer = std::unique_ptr<TType, Deleter<TType>>;
  }
}
