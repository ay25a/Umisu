#pragma once

#include "York/Core/result.hpp"
#include "York/Core/gui.hpp"
#include <wayland-client-core.h>

extern "C" {
#include <wayland-client.h>
#include <wayland-cursor.h>
#include "xdg-shell-client-protocol.h"
#include "zwlr-layer-shell-unstable-v1-client-protocol.h"
}

namespace york {

class Wayland;
template <>
struct PlatformTraits<Wayland> {
  using HandleType = wl_display *;
};

template <>
class Layer<Wayland> : public WindowBase<Wayland> {
public:
  static Result<Layer<Wayland> *> Create(const char *title, uint32_t width, uint32_t height);

private:
  Layer<Wayland>() = default;
  Error Init(const char *title, uint32_t width, uint32_t height) override;

public:
  ~Layer<Wayland>() override;
  Error Frame() const override;

public:
  struct WaylandState {
    wl_registry *registery = nullptr;
    wl_compositor *compositor = nullptr;
    wl_output *output = nullptr;
    wl_surface *surface = nullptr;
    xdg_wm_base *xdg = nullptr;
    zwlr_layer_shell_v1 *zwlr = nullptr;
    zwlr_layer_surface_v1 *zwlrSurface = nullptr;
  };

public:
  static void RegisteryAdd(void *data, struct wl_registry *, uint32_t name, const char *interface, uint32_t version);
  static constexpr wl_registry_listener REGESTRY_LISTENER{RegisteryAdd};

  static void ZWLRConfigure(void *data, zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t width, uint32_t height);
  static constexpr zwlr_layer_surface_v1_listener ZWLR_LISTENER{ZWLRConfigure};

  static void XDGPing(void *data, xdg_wm_base *wm_base, uint32_t serial) { xdg_wm_base_pong(wm_base, serial); }
  static constexpr xdg_wm_base_listener XDG_LISTENER{XDGPing};

private:
  WaylandState m_State;
};

} // namespace york