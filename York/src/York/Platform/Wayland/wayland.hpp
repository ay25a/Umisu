#pragma once

#include "York/Core/window.hpp"
extern "C" {
#include <wayland-client.h>
#include <xdg-shell-client-protocol.h>
#include <zwlr-layer-shell-unstable-v1-client-protocol.h>
}
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>

namespace york {
class Wayland;

struct WaylandState {
  wl_display *Display;
  wl_registry *Registery = nullptr;
  wl_compositor *Compositor = nullptr;
  wl_output *Output = nullptr;
  xdg_wm_base *XDG = nullptr;
  zwlr_layer_shell_v1 *ZWLR = nullptr;
};

template <>
struct PlatformTraits<Wayland> {
  static constexpr const char *VULKAN_EXTENSION_NAME = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
  using HandleType = wl_surface *;
  using LayerType = zwlr_layer_surface_v1 *;

  Result<VkSurfaceKHR> CreateSurface(VkInstance instance, HandleType handle);
};
} // namespace york