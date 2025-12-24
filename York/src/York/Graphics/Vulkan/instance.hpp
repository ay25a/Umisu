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

#include "York/Platform/Wayland/wayland.hpp"

namespace york::vulkan {

// Platform/Window/Debugging Extensions and Layers are automatically added in Instance::Create Function
struct InstanceCreateInfo {
  const char *AppName;
  uint32_t AppVersion = 0;
  const char *EngineName = "York";
  uint32_t EngineVersion = 0;
  APIVersion ApiVersion = APIVersion::V1_4;
  std::vector<const char *> Layers;
  std::vector<const char *> Extensions;
  bool EnableValidationLayers = false;
  bool EnableDebugMessenger = false;
};

// Severities is a combination of vulkan::DebugSeverity
// Types is a combination of vulkan::DebugTypes
// VK_DEBUG_UTILS_MESSAGE_* flags can be used as well
struct DebugMessengerCreateInfo {
  uint32_t Severities = 0;
  uint32_t Types = 0;
  UserDebugCallback *pDebugCallback = nullptr;
};

// Wrapper around VkInstance
// Additionally it handles Enumerating Physical Devices, and Enabling DebugMessenger Utilities
// Copy operators is removed to ensure having only one pointer to internal pointers at a time
class Instance {
public:
  // Platform template to handle automatically adding VK_*_SURFACE_EXTENSION_NAME
  // Shared Pointer is added to lightly manage lifetime of Instance
  template <typename Platform>
  static Result<std::shared_ptr<Instance>, Instance> Create(const InstanceCreateInfo &createInfo);

public:
  // Ensure the existance of all required Extensions and Layers
  // Create Error Message for Instance::GetInvalidLayers and Instance::GetInvalidExtensions
  static Status ValidateCreateInfo(const InstanceCreateInfo &createInfo);

  // Helper to Instance::ValidateCreateInfo
  static std::vector<std::string> GetInvalidLayers(const std::vector<const char *> &requested);

  // Helper to Instance::ValidateCreateInfo
  static std::vector<std::string> GetInvalidExtensions(const std::vector<const char *> &requested);

public:
  Result<void, VkDebugUtilsMessengerEXT> EnableDebugMessenger(const DebugMessengerCreateInfo &debugCreateInfo);

  // Enumerate and Parse Found Physical Devices
  // std::vector<>::size = 0 in case of an Error
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
  APIVersion m_APIVersion;
  VkInstance m_VkInstance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
};

// Specialization fpr Instance::Create template
extern template Result<std::shared_ptr<Instance>, Instance>
Instance::Create<Wayland>(const InstanceCreateInfo &);

} // namespace york::vulkan