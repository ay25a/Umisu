#pragma once

// clang-format off
#define ENUM_ENABLE_FLAGS(type) \
  inline uint32_t operator|(type a, type b) noexcept { return static_cast<uint32_t>(a) | static_cast<uint32_t>(b); } \
  inline uint32_t operator&(type a, type b) noexcept { return static_cast<uint32_t>(a) & static_cast<uint32_t>(b); }
