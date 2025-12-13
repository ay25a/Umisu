#pragma once

#include "York/Core/result.hpp"
#include <cstdint>

namespace york {

template <class Platform>
struct PlatformTraits;

template <class Platform>
class WindowBase {
public:
  using HandleType = typename PlatformTraits<Platform>::HandleType;

  struct Info {
    std::string title;
    uint32_t width = 0;
    uint32_t height = 0;
  };

protected:
  virtual Error Init(const char *title, uint32_t width, uint32_t height) = 0;

public:
  WindowBase<Platform>() = default;
  virtual ~WindowBase<Platform>() = 0;
  WindowBase<Platform>(const WindowBase<Platform> &) = delete;
  WindowBase<Platform> &operator=(const WindowBase<Platform> &) = delete;

  virtual Error Frame() const = 0;

protected:
  HandleType m_Handle;
  Info m_Info;
};

template <class Platform>
class Window : WindowBase<Platform> {};

template <class Platform>
class Layer : WindowBase<Platform> {};
} // namespace york