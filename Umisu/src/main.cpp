#include <York/Graphics/Vulkan/instance.hpp>
#include <York/Core/logger.hpp>

using namespace york;
int main() {
  york::Logger::init();
  // auto window = Layer<Wayland>::Create("Main Window", 800, 800);
  auto instance = vulkan::Instance::Create({
      .AppName = "Umisu",
      .EngineName = "York",
      .Extensions = {"NonExistant"},
      .EnableValidationLayers = true,
      .EnableDebugMessenger = true,
  });

  if (!instance)
    YORK_RUNTIME_LOG_ERROR(instance.error().type().name());

  return 0;
}