#pragma once

#include <cstdint>
#include <string>
#include "York/Helpers/general.hpp"
#include <vulkan/vulkan_core.h>

namespace york::vulkan {

enum class DebugSeverity : uint32_t {
  None = 0,
  Verbose = 1 << 0,
  Info = 1 << 1,
  Warning = 1 << 2,
  Error = 1 << 3,
};
ENUM_ENABLE_FLAGS(DebugSeverity)

enum class DebugType : uint32_t {
  None = 0,
  General = 1 << 0,
  Validation = 1 << 1,
  Performance = 1 << 2,
};
ENUM_ENABLE_FLAGS(DebugType)

typedef void(UserDebugCallback)(std::string severity, std::string type);

// clang-format off
  static PFN_vkDebugUtilsMessengerCallbackEXT BaseDebugCallback = [](
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData
  ) -> VkBool32 {
    
    std::string type;
    switch (messageTypes) {
      case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
      type = "VALIDATION";
      break;
      case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
      type = "PERFORMANCE";
      break;
      case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
      type = "GENERAL";
      break;
      default:
      type = "UNKNOWN";
      break;
    }
    
    std::string severity;
    switch (messageSeverity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      severity = "VERBOSE";
      break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      severity = "INFO";
      break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      severity = "WARNING";
      break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      severity = "ERROR";
      break;
      default:
      severity = "UNKNOWN";
      break;
    }
    
    UserDebugCallback* pUserDebugCallback = reinterpret_cast<UserDebugCallback*>(pUserData);
    (*pUserDebugCallback)(severity, type);
    
    return VK_FALSE;
  };

// clang-format on
struct DebugMessengerCreateInfo {
  DebugSeverity Severities = DebugSeverity::None;
  DebugType Types = DebugType::None;
  UserDebugCallback *pDebugCallback = nullptr;
};
} // namespace york::vulkan