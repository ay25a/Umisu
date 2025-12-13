#include "MikuAgent.hpp"
#include <cstdio>

Result<void> MikuAgent::Init() {
  {
    auto window = Window::Create("MikuAgent", 1920, 1080);
    if (!window)
      return std::unexpected(window.error());
    m_MainWindow = shared_ptr<Window>(window.value());
    printf("Agent: The Main Window is Created\n");
  }
  {
    auto vulkan = VulkanContext::Create(m_MainWindow.get());
    if (!vulkan)
      return std::unexpected(vulkan.error());
    m_Vulkan = shared_ptr<VulkanContext>(vulkan.value());
    printf("Agent: Vulkan Context is Created\n");
  }

  if (auto result = m_Renderer.Init(m_MainWindow, m_Vulkan); !result)
    return std::unexpected(result.error());

  printf("Agent: Initialized Components!\n");
  return {};
}

void MikuAgent::Run() {
  while (true) {
    m_Renderer.Frame([]() {});
    m_MainWindow->Frame();
  }
}