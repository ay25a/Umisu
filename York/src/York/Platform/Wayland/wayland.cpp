#include "York/Platform/Wayland/wayland.hpp"
#include "York/Graphics/Vulkan/helpers.hpp"

namespace york {
Result<VkSurfaceKHR> PlatformTraits<Wayland>::CreateSurface(VkInstance instance, HandleType handle) {
  extern WaylandState state;
  VkSurfaceKHR result = nullptr;

  VkWaylandSurfaceCreateInfoKHR surfaceCI{
      .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = {},
      .display = state.Display,
      .surface = handle,
  };

  if (auto code = vkCreateWaylandSurfaceKHR(instance, &surfaceCI, nullptr, &result); code != VK_SUCCESS)
    return failure<VkSurfaceKHR, VkSurfaceKHR>(vulkan::ToString(code), ErrorCode::VKCreation);

  return result;
}

} // namespace york