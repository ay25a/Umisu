#include "vulkan-context.hpp"

Result<VulkanContext *> VulkanContext::Create(Window *window) {
  VulkanContext *context = new VulkanContext();
  if (auto result = context->Init(window); !result) {
    delete context;
    return std::unexpected(result.error());
  }

  return context;
}

Result<void> VulkanContext::Init(Window *window) {
  try {
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    InitInstance(window);
    m_PhysicalDevice = m_Instance.enumeratePhysicalDevices().front();
    InitDevice();
    InitSurface(window);
  } catch (const std::exception &exc) {
    return std::unexpected(exc.what());
  }

  return {};
}

void VulkanContext::InitInstance(Window *window) {
  constexpr const char *LAYERS[] = {"VK_LAYER_KHRONOS_validation"};
  constexpr const char *EXTENSIONS[] = {
      vk::KHRSurfaceExtensionName,
      VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
  };

  const vk::ApplicationInfo ciApp(window->Title().data(), 1, ENGINE_NAME, 1, API_VERSION);
  const vk::InstanceCreateInfo ciInstance({}, &ciApp, 1, LAYERS, 2, EXTENSIONS);

  m_Instance = vk::createInstance(ciInstance);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);
}

void VulkanContext::InitDevice() {
  constexpr const char *EXTENSIONS[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_SPIRV_1_4_EXTENSION_NAME,
      VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
  };

  auto queueFamilies = m_PhysicalDevice.getQueueFamilyProperties();
  for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
    if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
      m_GraphicsIndex = i;
      break;
    }
  }

  float queuePriority = 1.0f;
  vk::DeviceQueueCreateInfo queueInfo({}, m_GraphicsIndex, 1, &queuePriority);

  vk::PhysicalDeviceDynamicRenderingFeatures drFeatures(vk::True);
  vk::PhysicalDeviceSynchronization2Features sync2Features(vk::True, &drFeatures);
  vk::PhysicalDeviceFeatures2 deviceFeatures2({}, sync2Features);

  vk::DeviceCreateInfo deviceInfo({}, 1, &queueInfo, 0, nullptr, 4, EXTENSIONS, nullptr, &deviceFeatures2);
  m_Device = m_PhysicalDevice.createDevice(deviceInfo);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device);

  m_GraphicsQueue = m_Device.getQueue(m_GraphicsIndex, 0);
}

void VulkanContext::InitSurface(Window *window) {
  VkWaylandSurfaceCreateInfoKHR surfaceInfo;
  memset(&surfaceInfo, 0, sizeof(VkWaylandSurfaceCreateInfoKHR));
  surfaceInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  surfaceInfo.display = window->Display();
  surfaceInfo.surface = window->Surface();

  VkSurfaceKHR surface = nullptr;
  if (vkCreateWaylandSurfaceKHR(m_Instance, &surfaceInfo, nullptr, &surface) != VK_SUCCESS)
    throw std::runtime_error("Could not create vulkan surface for wayland");

  m_Surface = surface;
}