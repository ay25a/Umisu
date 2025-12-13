#pragma once

#include "window.hpp"
#include "vulkan-context.hpp"
#include "renderer.hpp"

class MikuAgent {
public:
  MikuAgent() = default;
  ~MikuAgent() = default;

  Result<void> Init();
  void Run();

private:
  shared_ptr<Window> m_MainWindow;
  shared_ptr<VulkanContext> m_Vulkan;
  Renderer m_Renderer;
};