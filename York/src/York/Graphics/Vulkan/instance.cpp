#include "York/Graphics/Vulkan/instance.hpp"
#include "York/Helpers/strings.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace york::vulkan {

Result<std::unique_ptr<Instance>> Instance::Create(const InstanceCreateInfo &createInfo) {
  auto instance = std::unique_ptr<Instance>(new Instance());

  // clang-format off
  std::vector<const char *> layers{createInfo.Layers};
  if (createInfo.EnableValidationLayers) layers.emplace_back("VK_LAYER_KHRONOS_validation");
  
  std::vector<const char *> extensions{createInfo.Extensions};
  if (createInfo.EnableDebugMessenger) extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  // clang-format on

  if (auto error = instance->ValidateCreateInfo(createInfo); error) {
    return failure<std::unique_ptr<Instance>>("Failed to create Instance");
  }

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

  VK_CHECK(vkCreateInstance(&instanceCI, nullptr, &instance->m_VkInstance))
  instance->m_APIVersion = createInfo.ApiVersion;

  return ok(instance);
}

Error Instance::ValidateCreateInfo(const InstanceCreateInfo &createInfo) {
  auto layers = Instance::GetInvalidLayers(createInfo.Layers);
  auto extensions = Instance::GetInvalidExtensions(createInfo.Extensions);

  std::string error;
  if (layers.size() > 0)
    error += std::format("Requested Instance Layers are not available: {}\n", york::strings::join(layers));

  if (extensions.size() > 0)
    error += std::format("Requested Instance Extensions are not available: {}\n", york::strings::join(layers));

  if (!error.empty())
    return failure("{}", error);

  return ok();
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

Error Instance::EnableDebugMessenger(const DebugMessengerCreateInfo &ci) {
  // clang-format off
  uint32_t severities = 0;
  if(ci.Severities & DebugSeverity::Info) severities |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
  if(ci.Severities & DebugSeverity::Verbose) severities |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
  if(ci.Severities & DebugSeverity::Error) severities |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  if(ci.Severities & DebugSeverity::Warning) severities |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  
  uint32_t types = 0;
  if(ci.Types & DebugType::General) types |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  if(ci.Types & DebugType::Performance) types |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  if(ci.Types & DebugType::Validation) types |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  // clang-format on

  const VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = {},
      .messageSeverity = severities,
      .messageType = types,
      .pfnUserCallback = BaseDebugCallback,
      .pUserData = nullptr,
  };

  auto vkCreateDebugUtilsMessengerEXT =
      reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT"));

  VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_VkInstance, &debugCreateInfo, nullptr, &m_DebugMessenger));
  return ok();
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
} // namespace york::vulkan