#pragma once

#include "York/Core/result.hpp"
#include <vulkan/vulkan.h>

namespace york {
struct VulkanContextCreateInfo {
  const char **InstanceLayers = nullptr;
  uint32_t InstanceLayersCount = 0;
  const char **InstanceExtensions = nullptr;
  uint32_t InstanceExtensionsCount = 0;

  const char *AppName;
  uint32_t AppVersion = 0;
  const char *EngineName;
  uint32_t EngineVersion = 0;

  uint32_t ApiVersion = 0;

  const char **DeviceExtensions = nullptr;
  uint32_t DeviceExtensionsCount = 0;

  bool EnableValidationLayers = false;
  bool EnableDebugUtils = false;
};

class VulkanContext {
public:
  static Result<VulkanContext *> Create(const VulkanContextCreateInfo &info);

public:
  ~VulkanContext() = default;
  VulkanContext(const VulkanContext &) = delete;
  VulkanContext &operator=(const VulkanContext &) = delete;

private:
  VulkanContext() = default;

  Error CreateInstance(const VulkanContextCreateInfo &info);

private:
  VkInstance m_VkInstance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
  VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
  VkDevice m_Device = VK_NULL_HANDLE;

  VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
  uint32_t m_GraphicsQueueIndex = 0;
};
} // namespace york