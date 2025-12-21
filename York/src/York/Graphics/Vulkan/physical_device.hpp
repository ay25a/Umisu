#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "York/Helpers/general.hpp"

namespace york::vulkan {

enum class QueueRole : uint32_t {
  None = 0,
  Graphics = 1 << 0,
  Compute = 1 << 1,
  Present = 1 << 2,
};
ENUM_ENABLE_FLAGS(QueueRole)

struct QueueDesc {
  uint32_t Index = 0;
  QueueRole Roles = QueueRole::None;
};

struct PhysicalDevice {
  std::string Name;
  VkPhysicalDevice Handle = VK_NULL_HANDLE;
  uint32_t APIVersion = 0;
  uint32_t DriverVersion = 0;
  std::vector<QueueDesc> Queues;
};
} // namespace york::vulkan