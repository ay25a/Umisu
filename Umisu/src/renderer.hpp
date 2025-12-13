#pragma once

#include "core.hpp"
#include "window.hpp"
#include "vulkan-context.hpp"
#include <functional>

class Renderer {
public:
  static constexpr std::array<float, 4> CLEAR_COLOR{0.0f, 0.0f, 0.0f, 0.0f};

public:
  Renderer() = default;
  ~Renderer() = default;

  Result<void> Init(shared_ptr<Window> window, shared_ptr<VulkanContext> vulkan);

  Result<void> Frame(const std::function<void()> &render);

private:
  void CreateSwapchain();
  void CreateImages();
  void CreateCommands();
  void CreateSyncs();

private:
  uint32_t m_FramesInFlight = 0;
  shared_ptr<Window> m_TargetWindow;
  shared_ptr<VulkanContext> m_VulkanContext;

  vk::SwapchainCreateInfoKHR m_SwapchainCI;
  vk::SwapchainKHR m_Swapchain;
  std::vector<vk::Image> m_Images;
  std::vector<vk::ImageView> m_ImageViews;

  std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
  std::vector<vk::Semaphore> m_RenderFinishedSemaphore;
  std::vector<vk::Fence> m_InFlightFences;

  vk::CommandPool m_CommandPool;
  std::vector<vk::CommandBuffer> m_CommandBuffers;
};