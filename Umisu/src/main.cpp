#include <York/Graphics/Vulkan/instance.hpp>
using namespace york;
int main() {
  // auto window = Layer<Wayland>::Create("Main Window", 800, 800);
  auto instance = vulkan::Instance::Create({
      .AppName = "Umisu",
      .EngineName = "York",
      .EnableValidationLayers = true,
      .EnableDebugMessenger = true,
  });
  return 0;
}