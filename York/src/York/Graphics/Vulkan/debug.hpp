#pragma once

/*
 * Vulkan Debug Messenger Utilities and Wrappers
 */

#include <cstdint>
#include <string>
#include <functional>
#include "York/Helpers/general.hpp"
#include <vulkan/vulkan_core.h>

namespace york::vulkan {

// Wrapper around VK_DEBUG_UTILS_MESSAGE_SEVERITY_*
enum class DebugSeverity : uint32_t {
  None = 0,
  Verbose = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
  Info = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
  Warning = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
  Error = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
};
ENUM_ENABLE_FLAGS(DebugSeverity)

// Wrapper around VK_DEBUG_UTILS_MESSAGE_TYPE_*
enum class DebugType : uint32_t {
  None = 0,
  General = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
  Validation = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
  Performance = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
};
ENUM_ENABLE_FLAGS(DebugType)

// Type specifier for user-defined debug callback
using UserDebugCallback = std::function<void(std::string_view severity, std::string_view type, std::string_view msg)>;

// Base debug callback function
// clang-format off
static PFN_vkDebugUtilsMessengerCallbackEXT BaseDebugCallback = [](
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
  VkDebugUtilsMessageTypeFlagsEXT messageTypes,
  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
  void *pUserData) -> VkBool32 {
    std::string type;
    switch (messageTypes) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:  type = "VALIDATION";  break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: type = "PERFORMANCE"; break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:     type = "GENERAL";     break;
    default:                                              type = "UNKNOWN";     break;
    }
    
    std::string severity;
    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: severity = "VERBOSE"; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    severity = "INFO";    break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: severity = "WARNING"; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   severity = "ERROR";   break;
    default:                                              severity = "UNKNOWN"; break;
    }
    
    UserDebugCallback* pUserDebugCallback = reinterpret_cast<UserDebugCallback*>(pUserData);
    (*pUserDebugCallback)(severity, type, pCallbackData->pMessage);
    
    return VK_FALSE;
  };
// clang-format on
} // namespace york::vulkan