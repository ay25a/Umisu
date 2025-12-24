#include "York/Graphics/Vulkan/instance.hpp"
#include "York/Core/error.hpp"
#include "York/Core/gui.hpp"
#include "York/Core/result.hpp"
#include "York/Helpers/strings.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace york::vulkan {

// =====================
// Instance Creation
// =====================
template <typename Platform>
Result<std::shared_ptr<Instance>, Instance> Instance::Create(const InstanceCreateInfo &createInfo) {
  auto instance = std::shared_ptr<Instance>(new Instance());

  // clang-format off
  std::vector<const char *> layers{createInfo.Layers};
  if (createInfo.EnableValidationLayers) layers.emplace_back("VK_LAYER_KHRONOS_validation");
  
  std::vector<const char *> extensions{createInfo.Extensions};
  if (createInfo.EnableDebugMessenger) extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
  extensions.emplace_back(PlatformTraits<Platform>::VULKAN_EXTENSION_NAME);
  // clang-format on

  if (auto status = instance->ValidateCreateInfo(createInfo); !status.has_value())
    return failure<std::unique_ptr<Instance>, Instance>(status, ErrorCode::VKValidation);

  const VkApplicationInfo appCI{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = createInfo.AppName,
      .applicationVersion = createInfo.AppVersion,
      .pEngineName = createInfo.EngineName,
      .engineVersion = createInfo.EngineVersion,
      .apiVersion = york::version::to_vulkan(static_cast<uint32_t>(createInfo.ApiVersion)),
  };

  const VkInstanceCreateInfo instanceCI{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = {},
      .pApplicationInfo = &appCI,
      .enabledLayerCount = static_cast<uint32_t>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
  };

  if (auto code = vkCreateInstance(&instanceCI, nullptr, &instance->m_VkInstance); code != VK_SUCCESS)
    return failure<std::unique_ptr<Instance>, Instance>(ToString(code), ErrorCode::VKCreation);

  instance->m_APIVersion = createInfo.ApiVersion;
  return instance;
}

template Result<std::shared_ptr<Instance>, Instance>
Instance::Create<Wayland>(const InstanceCreateInfo &createInfo);

// =====================
// Create Info Validation
// =====================
Status Instance::ValidateCreateInfo(const InstanceCreateInfo &createInfo) {
  auto layers = Instance::GetInvalidLayers(createInfo.Layers);
  auto extensions = Instance::GetInvalidExtensions(createInfo.Extensions);

  std::string error;
  if (layers.size() > 0)
    error += std::format("Requested Instance Layers are not available: {}\n", york::strings::join(layers));

  if (extensions.size() > 0)
    error += std::format("Requested Instance Extensions are not available: {}\n", york::strings::join(layers));

  if (!error.empty())
    return failure("Invalid Instance Create Info:\n{}", error);

  return STATUS_SUCCESS;
}

std::vector<std::string> Instance::GetInvalidLayers(const std::vector<const char *> &requested) {
  uint32_t count(0);
  vkEnumerateInstanceLayerProperties(&count, nullptr);
  std::vector<VkLayerProperties> layers(count);
  vkEnumerateInstanceLayerProperties(&count, layers.data());

  std::vector<std::string> invalid;
  for (const auto &layer : requested) {
    bool found = false;
    for (const auto &props : layers) {
      found = props.layerName == std::string_view(layer);
      if (found)
        break;
    }

    if (!found)
      invalid.emplace_back(layer);
  }

  return invalid;
}

std::vector<std::string> Instance::GetInvalidExtensions(const std::vector<const char *> &requested) {
  uint32_t count(0);
  vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> extensions(count);
  vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

  std::vector<std::string> invalid;
  for (const auto &ext : requested) {
    bool found = false;
    for (const auto &props : extensions) {
      found = props.extensionName == std::string_view(ext);
      if (found)
        break;
    }

    if (!found)
      invalid.emplace_back(ext);
  }

  return invalid;
}

// =====================
// Runtime Operations
// =====================

Result<void, VkDebugUtilsMessengerEXT> Instance::EnableDebugMessenger(const DebugMessengerCreateInfo &ci) {
  const VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = {},
      .messageSeverity = ci.Severities,
      .messageType = ci.Types,
      .pfnUserCallback = BaseDebugCallback,
      .pUserData = nullptr,
  };

  auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT"));

  if (auto code = vkCreateDebugUtilsMessengerEXT(m_VkInstance, &debugCreateInfo, nullptr, &m_DebugMessenger); code != VK_SUCCESS)
    return failure<void, VkDebugUtilsMessengerEXT>(ToString(code), ErrorCode::VKCreation);

  return {};
}

std::vector<PhysicalDevice> Instance::EnumeratePhysicalDevices() const {
  uint32_t count(0);
  vkEnumeratePhysicalDevices(m_VkInstance, &count, nullptr);
  std::vector<VkPhysicalDevice> devices(count);
  vkEnumeratePhysicalDevices(m_VkInstance, &count, devices.data());

  std::vector<PhysicalDevice> result;
  for (const auto &device : devices) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);

    PhysicalDevice parsed{
        .Name = props.deviceName,
        .Handle = device,
        .APIVersion = york::version::from_vulkan(props.apiVersion),
        .DriverVersion = york::version::from_vulkan(props.driverVersion),
    };

    count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> queues(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queues.data());

    for (uint32_t i(0); i < queues.size(); ++i) {
      QueueRole roles = QueueRole::None;

      if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        roles = static_cast<QueueRole>(roles | QueueRole::Graphics);
      if (queues[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        roles = static_cast<QueueRole>(roles | QueueRole::Compute);

      parsed.Queues.emplace_back(i, roles);
    }

    result.emplace_back(parsed);
  }
  return result;
}

// =====================
// Destructor
// =====================
Instance::~Instance() {
  if (m_DebugMessenger) {
    auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT"));
    vkDestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMessenger, nullptr);
  }
  if (m_VkInstance)
    vkDestroyInstance(m_VkInstance, nullptr);
}
} // namespace york::vulkan