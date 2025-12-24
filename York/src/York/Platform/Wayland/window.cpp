#include "York/Core/window.hpp"
#include "York/Core/error.hpp"
#include "York/Core/result.hpp"
#include "York/Platform/Wayland/wayland.hpp"
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

namespace york {
template <>
uint32_t Window<Wayland>::s_WindowCount = 0;

WaylandState g_SharedState;

static void RegisteryAdd(void *data, struct wl_registry *, uint32_t name, const char *interface, uint32_t version);
static constexpr wl_registry_listener REGESTRY_LISTENER{RegisteryAdd};

static void XDGPing(void *data, xdg_wm_base *wm_base, uint32_t serial) { xdg_wm_base_pong(wm_base, serial); }
static constexpr xdg_wm_base_listener XDG_LISTENER{XDGPing};

static void ZWLRConfigure(void *data, zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t width, uint32_t height);
static constexpr zwlr_layer_surface_v1_listener ZWLR_LISTENER{ZWLRConfigure};

template <>
Result<> Window<Wayland>::MakeLayer() {
  // clang-format off
    do { wl_display_roundtrip(g_SharedState.Display); }
    while (!g_SharedState.ZWLR);
  // clang-format on

  bool ready = false;
  m_Layer = zwlr_layer_shell_v1_get_layer_surface(g_SharedState.ZWLR, m_Handle, g_SharedState.Output, ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY, m_CreateInfo.Title.c_str());
  if (!m_Layer)
    return YK_RESULT_FAILURE(Error::Create("zwlr_layer_shell_v1_get_layer_surface failed"));

  zwlr_layer_surface_v1_set_size(m_Layer, m_CreateInfo.Width, m_CreateInfo.Height);
  zwlr_layer_surface_v1_set_anchor(m_Layer, ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM);
  zwlr_layer_surface_v1_add_listener(m_Layer, &ZWLR_LISTENER, static_cast<void *>(&ready));

  wl_surface_set_opaque_region(m_Handle, nullptr);
  wl_region *region = wl_compositor_create_region(g_SharedState.Compositor);
  wl_surface_set_input_region(m_Handle, region);
  wl_region_destroy(region);

  wl_surface_commit(m_Handle);

  // clang-format off
  do { wl_display_roundtrip(g_SharedState.Display); } 
  while (!ready);
  // clang-format on

  return YK_RESULT_SUCCESS({});
}

template <>
Result<> Window<Wayland>::Init() {
  if (Window<Wayland>::s_WindowCount == 0) {
    if (g_SharedState.Display = wl_display_connect(nullptr); !g_SharedState.Display)
      return YK_RESULT_FAILURE(Error::Create("wl_display_connect failed"));

    if (g_SharedState.Registery = wl_display_get_registry(g_SharedState.Display); !g_SharedState.Registery)
      return YK_RESULT_FAILURE(Error::Create("wl_display_get_registry failed"));

    wl_registry_add_listener(g_SharedState.Registery, &REGESTRY_LISTENER, nullptr);

    // clang-format off
    do { wl_display_roundtrip(g_SharedState.Display); }
    while (!g_SharedState.Compositor || !g_SharedState.XDG || !g_SharedState.Output);
    // clang-format on

    xdg_wm_base_add_listener(g_SharedState.XDG, &XDG_LISTENER, nullptr);
  }

  if (m_Handle = wl_compositor_create_surface(g_SharedState.Compositor); !m_Handle)
    return YK_RESULT_FAILURE(Error::Create("wl_compositor_create_surface failed"));

  if (m_CreateInfo.IsLayer)
    if (auto result = MakeLayer(); !result)
      return YK_RESULT_FAILURE(result.error());

  wl_surface_commit(m_Handle);
  return YK_RESULT_SUCCESS({});
}

template <>
Result<std::unique_ptr<Window<Wayland>>> Window<Wayland>::Create(const WindowCreateInfo &ci) {
  auto window = std::unique_ptr<Window<Wayland>>(new Window<Wayland>);

  window->m_CreateInfo = ci;
  if (auto result = window->Init(); !result)
    return YK_RESULT_FAILURE(result.error());

  Window<Wayland>::s_WindowCount++;
  return YK_RESULT_SUCCESS(window);
}

template <>
void Window<Wayland>::Frame() const {
  wl_display_dispatch(g_SharedState.Display);
  wl_display_flush(g_SharedState.Display);
  wl_surface_commit(m_Handle);
}

template <>
Window<Wayland>::~Window() {
  if (m_Layer)
    zwlr_layer_surface_v1_destroy(m_Layer);

  if (m_Handle) {
    wl_surface_destroy(m_Handle);
    Window<Wayland>::s_WindowCount--;
  }

  if (Window<Wayland>::s_WindowCount == 0) {
    wl_display_disconnect(g_SharedState.Display);
    g_SharedState = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
  }
}

void RegisteryAdd(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
  if (std::string_view(interface) == wl_compositor_interface.name)
    g_SharedState.Compositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, version));
  else if (std::string_view(interface) == xdg_wm_base_interface.name)
    g_SharedState.XDG = static_cast<xdg_wm_base *>(wl_registry_bind(registry, name, &xdg_wm_base_interface, version));
  else if (std::string_view(interface) == zwlr_layer_shell_v1_interface.name)
    g_SharedState.ZWLR = static_cast<zwlr_layer_shell_v1 *>(wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, version));
  else if (std::string_view(interface) == wl_output_interface.name)
    g_SharedState.Output = static_cast<wl_output *>(wl_registry_bind(registry, name, &wl_output_interface, version));
}

void ZWLRConfigure(void *data, zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t width, uint32_t height) {
  zwlr_layer_surface_v1_ack_configure(surface, serial);
  bool *configured = static_cast<bool *>(data);
  *configured = true;
}

} // namespace york