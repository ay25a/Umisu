#include "York/Platform/Wayland/layer.hpp"
#include "York/Core/error.hpp"
#include "York/Core/gui.hpp"
#include "York/Core/result.hpp"
#include <memory>

namespace york {

Result<std::unique_ptr<Layer<Wayland>>, Layer<Wayland>>
Layer<Wayland>::Create(const char *title, uint32_t width, uint32_t height) {
  auto layer = std::unique_ptr<Layer<Wayland>>(new Layer<Wayland>);

  if (auto status = layer->Init(title, width, height); !status.has_value())
    return failure<std::unique_ptr<Layer<Wayland>>, Layer<Wayland>>(status, ErrorCode::WindowCreation);

  layer->m_Info = {
      .title = title,
      .width = width,
      .height = height,
  };

  return layer;
}

Status Layer<Wayland>::Init(const char *title, uint32_t width, uint32_t height) {
  if (m_Handle = wl_display_connect(nullptr); !m_Handle)
    return failure("wl_display_connect failed");

  if (m_State.registery = wl_display_get_registry(m_Handle); !m_State.registery)
    return failure("wl_display_get_registry failed");

  wl_registry_add_listener(m_State.registery, &REGESTRY_LISTENER, static_cast<void *>(&m_State));

  // clang-format off
  do { wl_display_roundtrip(m_Handle);} 
  while (!m_State.compositor || !m_State.xdg || !m_State.zwlr || !m_State.output);
  // clang-format on

  xdg_wm_base_add_listener(m_State.xdg, &XDG_LISTENER, nullptr);
  if (m_State.surface = wl_compositor_create_surface(m_State.compositor); !m_State.surface)
    return failure("wl_compositor_create_surface failed");

  bool ready = false;
  m_State.zwlrSurface = zwlr_layer_shell_v1_get_layer_surface(
      m_State.zwlr, m_State.surface, m_State.output, ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY, title);
  if (!m_State.zwlrSurface)
    return failure("zwlr_layer_shell_v1_get_layer_surface failed");

  zwlr_layer_surface_v1_set_size(m_State.zwlrSurface, width, height);
  zwlr_layer_surface_v1_set_anchor(m_State.zwlrSurface, ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM);
  zwlr_layer_surface_v1_add_listener(m_State.zwlrSurface, &ZWLR_LISTENER, static_cast<void *>(&ready));

  wl_surface_set_opaque_region(m_State.surface, nullptr);
  wl_region *region = wl_compositor_create_region(m_State.compositor);
  wl_surface_set_input_region(m_State.surface, region);
  wl_region_destroy(region);

  wl_surface_commit(m_State.surface);

  // clang-format off
  do { wl_display_roundtrip(m_Handle); } 
  while (!ready);
  // clang-format on

  return STATUS_SUCCESS;
}

Layer<Wayland>::~Layer() {
  if (m_Handle) {
    zwlr_layer_surface_v1_destroy(m_State.zwlrSurface);
    wl_surface_destroy(m_State.surface);
    wl_display_disconnect(m_Handle);
  }
}

Status Layer<Wayland>::Frame() const {
  wl_display_dispatch(m_Handle);
  wl_display_flush(m_Handle);
  wl_surface_commit(m_State.surface);

  return STATUS_SUCCESS;
}

void Layer<Wayland>::RegisteryAdd(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
  if (std::string_view(interface) == wl_compositor_interface.name) {
    auto state = static_cast<Layer<Wayland>::WaylandState *>(data);
    state->compositor = static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, version));
  } else if (std::string_view(interface) == xdg_wm_base_interface.name) {
    auto state = static_cast<Layer<Wayland>::WaylandState *>(data);
    state->xdg = static_cast<xdg_wm_base *>(wl_registry_bind(registry, name, &xdg_wm_base_interface, version));
  } else if (std::string_view(interface) == zwlr_layer_shell_v1_interface.name) {
    auto state = static_cast<Layer<Wayland>::WaylandState *>(data);
    state->zwlr = static_cast<zwlr_layer_shell_v1 *>(wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, version));
  } else if (std::string_view(interface) == wl_output_interface.name) {
    auto state = static_cast<Layer<Wayland>::WaylandState *>(data);
    state->output = static_cast<wl_output *>(wl_registry_bind(registry, name, &wl_output_interface, version));
  }
}

void Layer<Wayland>::ZWLRConfigure(void *data, zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t width, uint32_t height) {
  zwlr_layer_surface_v1_ack_configure(surface, serial);
  bool *configured = static_cast<bool *>(data);
  *configured = true;
}

} // namespace york