#include "York/Graphics/Vulkan/debug.hpp"
#include <York/Graphics/Vulkan/instance.hpp>
#include <York/Platform/Wayland/layer.hpp>
#include <York/Core/logger.hpp>
#include <memory>
#include <string_view>

using namespace york;

static vulkan::UserDebugCallback DebugCallback = [](std::string_view severity, std::string_view type, std::string_view msg) {
  YORK_VULKAN_LOG_CRITICAL("DebugMessenger [{} & {}]: {}", severity, type, msg);
};

int main() {
  york::Logger::init();

  std::unique_ptr<Layer<Wayland>> window;
  {
    auto result = Layer<Wayland>::Create("Main Window", 800, 800);
    if (!result) {
      YORK_RUNTIME_LOG_CRITICAL(result.error().message);
      return -1;
    }
    result->swap(window);
  }
  std::shared_ptr<vulkan::Instance> instance;
  {
    auto result = vulkan::Instance::Create<Wayland>({
        .AppName = "Umisu",
        .EngineName = "York",
        .EnableValidationLayers = true,
        .EnableDebugMessenger = true,
    });

    if (!result) {
      YORK_RUNTIME_LOG_CRITICAL(result.error().message);
      return -1;
    }
    result->swap(instance);
  }

  {
    auto result = instance->EnableDebugMessenger({
        .Severities = vulkan::DebugSeverity::Error | vulkan::DebugSeverity::Warning | vulkan::DebugSeverity::Info,
        .Types = vulkan::DebugType::General | vulkan::DebugType::Validation | vulkan::DebugType::Performance,
        .pDebugCallback = &DebugCallback,
    });

    if (!result) {
      YORK_RUNTIME_LOG_CRITICAL(result.error().message);
      return -1;
    }
  }

  auto d = instance->EnumeratePhysicalDevices();

  while (true) {
    if (auto status = window->Frame(); !status.has_value()) {
      YORK_ENGINE_LOG_CRITICAL(status.error());
      return -1;
    }
  }

  return 0;
}