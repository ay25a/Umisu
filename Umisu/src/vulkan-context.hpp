#pragma once

#include "core.hpp"
#include "window.hpp"
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_wayland.h>

class VulkanContext {
public:
  static constexpr uint32_t API_VERSION = vk::ApiVersion14;
  static constexpr const char *ENGINE_NAME = "NONE";

  static Result<VulkanContext *> Create(Window *window);

private:
  VulkanContext() = default;

  Result<void> Init(Window *window);
  void InitInstance(Window *window);
  void InitDevice();
  void InitSurface(Window *window);

public:
  ~VulkanContext() = default;

public:
  auto &Surface() const noexcept { return m_Surface; }
  auto &Device() const noexcept { return m_Device; }
  auto &PhysicalDevice() const noexcept { return m_PhysicalDevice; }
  auto QueueIndex() const noexcept { return m_GraphicsIndex; }
  auto &Queue() const noexcept { return m_GraphicsQueue; }

private:
  vk::Instance m_Instance;
  vk::PhysicalDevice m_PhysicalDevice;
  vk::SurfaceKHR m_Surface;
  vk::Queue m_GraphicsQueue;
  uint32_t m_GraphicsIndex = 0;
  vk::Device m_Device;
};