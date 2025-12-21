#pragma once

#include <vulkan/vulkan_core.h>
#include <optional>
#include <vector>
#include <memory>
#include "York/Core/result.hpp"
#include "York/Helpers/version.hpp"
#include "York/Graphics/Vulkan/debug.hpp"
#include "York/Graphics/Vulkan/helpers.hpp"
#include "York/Graphics/Vulkan/physical_device.hpp"

namespace york::vulkan {

struct InstanceCreateInfo {
  const char *AppName;
  uint32_t AppVersion = 0;
  const char *EngineName;
  uint32_t EngineVersion = 0;
  APIVersion ApiVersion = APIVersion::NONE;
  std::vector<const char *> Layers;
  std::vector<const char *> Extensions;
  bool EnableValidationLayers = false;
  bool EnableDebugMessenger = false;
};

class Instance {
public:
  static Result<std::unique_ptr<Instance>> Create(const InstanceCreateInfo &createInfo);

public:
  static Error ValidateCreateInfo(const InstanceCreateInfo &createInfo);
  static std::vector<std::string> GetInvalidLayers(const std::vector<const char *> &requested);
  static std::vector<std::string> GetInvalidExtensions(const std::vector<const char *> &requested);

  Error EnableDebugMessenger(const DebugMessengerCreateInfo &debugCreateInfo);
  std::vector<PhysicalDevice> EnumeratePhysicalDevices() const;

private:
  Instance() = default;

public:
  ~Instance();
  Instance(const Instance &) = delete;
  Instance &operator=(const Instance &) = delete;

public:
  APIVersion GetVersion() const noexcept { return m_APIVersion; }
  VkInstance Get() const noexcept { return m_VkInstance; }

private:
  APIVersion m_APIVersion = APIVersion::NONE;
  VkInstance m_VkInstance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
};

} // namespace york::vulkan