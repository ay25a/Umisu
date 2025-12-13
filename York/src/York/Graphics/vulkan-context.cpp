#include "vulkan-context.hpp"

namespace york {
Result<VulkanContext *> VulkanContext::Create(const VulkanContextCreateInfo &info) {
  auto context = new VulkanContext();

  return context;
}

Error VulkanContext::CreateInstance(const VulkanContextCreateInfo &info) {
  const VkApplicationInfo appCI{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = info.AppName,
      .applicationVersion = info.AppVersion,
      .pEngineName = info.EngineName,
      .engineVersion = info.EngineVersion,
      .apiVersion = info.ApiVersion,
  };
  const VkInstanceCreateInfo instanceCI{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appCI,
      .enabledLayerCount = info.InstanceLayersCount,
      .ppEnabledLayerNames = info.InstanceLayers,
      .enabledExtensionCount = info.InstanceExtensionsCount,
      .ppEnabledExtensionNames = info.InstanceExtensions,
  };

  VkResult result = vkCreateInstance(&instanceCI, nullptr, &m_VkInstance);
  if (result != VK_SUCCESS) {
    return YORK_FAILURE("Failed to create Vulkan instance, code: {}", static_cast<uint32_t>(result));
  }

  return YORK_OK();
}
} // namespace york