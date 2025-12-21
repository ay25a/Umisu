#pragma once

#include <string>
#include <format>
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace york::version {

inline constexpr uint32_t major(uint32_t version) noexcept { return version >> 22U; }
inline constexpr uint32_t minor(uint32_t version) noexcept { return (version >> 12U) & 0x3FFU; }
inline constexpr uint32_t patch(uint32_t version) noexcept { return version & 0xFFFU; }

inline constexpr uint32_t make(uint32_t major, uint32_t minor, uint32_t patch = 0) {
  return (major << 22U) | (minor << 12U) | patch;
}

inline constexpr std::string to_string(uint32_t version) {
  return std::format("{}.{}.{}", major(version), minor(version), patch(version));
}

inline constexpr uint32_t from_vulkan(uint32_t version) {
  return make(VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
}

inline constexpr uint32_t to_vulkan(uint32_t version) {
  return VK_MAKE_VERSION(major(version), minor(version), patch(version));
}

inline constexpr uint32_t to_vulkan(uint32_t major, uint32_t minor, uint32_t patch = 0) {
  return VK_MAKE_VERSION(major, minor, patch);
}

} // namespace york::version