#pragma once

#include "York/Core/result.hpp"
#include <cstdint>
#include <memory>

namespace york {
// Usage:
// using HandleType = <Window Handle Type>
// using LayerType = <Layered Window Type or std::nullptr_t>
// constexpr VK_SURFACE_EXTENSION_NAME = '<Vulkan Surface Extension name>'
// VkSurface CreateSurface(HandleType handle, VkInstance instance)
template <class Platform>
struct PlatformTraits;

struct WindowCreateInfo {
  std::string Title;
  uint32_t Width = 0;
  uint32_t Height = 0;
  bool IsLayer;
};

// Example of Definition of Platform: { class Windows; } & { class Wayland; }
// Used by definition of methods: ex. Window<Wayland>::Create(){}
template <class Platform>
class Window {
public:
  static uint32_t s_WindowCount;
  using HandleType = typename PlatformTraits<Platform>::HandleType;
  using LayerType = typename PlatformTraits<Platform>::LayerType;

public:
  static Result<std::unique_ptr<Window>> Create(const WindowCreateInfo &ci);

private:
  Window<Platform>() = default;

  Result<> Init();
  Result<> MakeLayer();

public:
  ~Window<Platform>();
  Window<Platform>(const Window<Platform> &) = delete;
  Window<Platform> &operator=(const Window<Platform> &) = delete;

  void Frame() const;

protected:
  HandleType m_Handle;
  WindowCreateInfo m_CreateInfo;
  LayerType m_Layer;
};
} // namespace york