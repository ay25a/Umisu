#include "renderer.hpp"

Result<void> Renderer::Init(shared_ptr<Window> window, shared_ptr<VulkanContext> vulkan) {
  m_TargetWindow = window;
  m_VulkanContext = vulkan;

  try {
    CreateSwapchain();
    CreateImages();
    CreateCommands();
    CreateSyncs();
  } catch (const vk::Error &exc) {
    return std::unexpected(exc.what());
  }

  return {};
}

void Renderer::CreateSwapchain() {
  auto caps = m_VulkanContext->PhysicalDevice().getSurfaceCapabilitiesKHR(m_VulkanContext->Surface());
  auto fmt = m_VulkanContext->PhysicalDevice().getSurfaceFormatsKHR(m_VulkanContext->Surface()).front();

  uint32_t queueIndex = m_VulkanContext->QueueIndex();

  m_FramesInFlight = caps.minImageCount;
  m_SwapchainCI.surface = m_VulkanContext->Surface();
  m_SwapchainCI.minImageCount = caps.minImageCount;
  m_SwapchainCI.imageFormat = fmt.format;
  m_SwapchainCI.imageColorSpace = fmt.colorSpace;
  m_SwapchainCI.imageExtent = vk::Extent2D(m_TargetWindow->Width(), m_TargetWindow->Height());
  m_SwapchainCI.imageArrayLayers = 1;
  m_SwapchainCI.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
  m_SwapchainCI.imageSharingMode = vk::SharingMode::eExclusive;
  m_SwapchainCI.queueFamilyIndexCount = 1;
  m_SwapchainCI.pQueueFamilyIndices = &queueIndex;
  m_SwapchainCI.preTransform = caps.currentTransform;
  m_SwapchainCI.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
  m_SwapchainCI.presentMode = vk::PresentModeKHR::eFifo;
  m_SwapchainCI.clipped = vk::True;

  m_Swapchain = m_VulkanContext->Device().createSwapchainKHR(m_SwapchainCI);
}

void Renderer::CreateImages() {
  m_Images = m_VulkanContext->Device().getSwapchainImagesKHR(m_Swapchain);

  vk::ImageViewCreateInfo ivInfo{};
  ivInfo.viewType = vk::ImageViewType::e2D;
  ivInfo.format = m_VulkanContext->PhysicalDevice().getSurfaceFormatsKHR(m_VulkanContext->Surface()).front().format;
  ivInfo.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

  m_ImageViews.resize(m_Images.size());
  for (uint16_t i(0); i < m_Images.size(); ++i) {
    ivInfo.image = m_Images[i];
    m_ImageViews[i] = m_VulkanContext->Device().createImageView(ivInfo);
  }
}

void Renderer::CreateCommands() {
  vk::CommandPoolCreateInfo cpInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_VulkanContext->QueueIndex());
  m_CommandPool = m_VulkanContext->Device().createCommandPool(cpInfo);

  vk::CommandBufferAllocateInfo cbInfo(m_CommandPool, vk::CommandBufferLevel::ePrimary, m_FramesInFlight);
  m_CommandBuffers = m_VulkanContext->Device().allocateCommandBuffers(cbInfo);
}

void Renderer::CreateSyncs() {
  vk::SemaphoreCreateInfo spInfo{};
  vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

  m_InFlightFences.resize(m_FramesInFlight);
  m_ImageAvailableSemaphores.resize(m_FramesInFlight);
  m_RenderFinishedSemaphore.resize(m_FramesInFlight);
  for (uint16_t i(0); i < m_FramesInFlight; ++i) {
    m_ImageAvailableSemaphores[i] = m_VulkanContext->Device().createSemaphore(spInfo);
    m_RenderFinishedSemaphore[i] = m_VulkanContext->Device().createSemaphore(spInfo);
    m_InFlightFences[i] = m_VulkanContext->Device().createFence(fenceInfo);
  }
}

Result<void> Renderer::Frame(const std::function<void()> &render) {
  static uint32_t currentFrame = 0;

  auto &fence = m_InFlightFences[currentFrame];
  auto &spImageAvailable = m_ImageAvailableSemaphores[currentFrame];
  auto &spRenderFinished = m_RenderFinishedSemaphore[currentFrame];
  auto &cb = m_CommandBuffers[currentFrame];

  if (m_VulkanContext->Device().waitForFences(1, &fence, vk::True, UINT64_WIDTH) != vk::Result::eSuccess)
    return std::unexpected("Failed to wait for a fence");

  m_VulkanContext->Device().resetFences(fence);

  uint32_t imageIndex = 0;
  if (m_VulkanContext->Device().acquireNextImageKHR(
          m_Swapchain, UINT64_MAX, spImageAvailable, nullptr, &imageIndex) != vk::Result::eSuccess) {
    return std::unexpected("Failed to acquire next image");
  }

  cb.reset();
  cb.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  // Barrier
  vk::ImageMemoryBarrier2 bToColor(
      vk::PipelineStageFlagBits2::eNone,
      vk::AccessFlagBits2::eNone,
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      vk::AccessFlagBits2::eColorAttachmentWrite,
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eColorAttachmentOptimal,
      m_VulkanContext->QueueIndex(), m_VulkanContext->QueueIndex(), m_Images[imageIndex],
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
  vk::DependencyInfo btcDep{};
  btcDep.imageMemoryBarrierCount = 1;
  btcDep.pImageMemoryBarriers = &bToColor;
  cb.pipelineBarrier2(btcDep);

  // Dynamic rendering
  vk::RenderingAttachmentInfo colorAttachment(m_ImageViews[imageIndex]);
  colorAttachment.imageView = m_ImageViews[imageIndex];
  colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
  colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
  colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
  colorAttachment.clearValue = vk::ClearValue(CLEAR_COLOR);

  vk::RenderingInfo renderingInfo;
  renderingInfo.renderArea = vk::Rect2D(vk::Offset2D{0, 0}, vk::Extent2D(m_TargetWindow->Width(), m_TargetWindow->Height()));
  renderingInfo.layerCount = 1;
  renderingInfo.viewMask = 0;
  renderingInfo.colorAttachmentCount = 1;
  renderingInfo.pColorAttachments = &colorAttachment;

  cb.beginRendering(&renderingInfo);
  render();
  cb.endRendering();

  // Barrier
  vk::ImageMemoryBarrier2 bToPresent(
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      vk::AccessFlagBits2::eColorAttachmentWrite,
      vk::PipelineStageFlagBits2::eBottomOfPipe,
      vk::AccessFlagBits2::eNone,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::ImageLayout::ePresentSrcKHR,
      0, 0,
      m_Images[imageIndex],
      {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

  vk::DependencyInfo btpDep{};
  btpDep.imageMemoryBarrierCount = 1;
  btpDep.pImageMemoryBarriers = &bToPresent;
  cb.pipelineBarrier2(btpDep);

  // Submit
  cb.end();
  auto wdsmbits = vk::Flags<vk::PipelineStageFlagBits>(vk::PipelineStageFlagBits::eColorAttachmentOutput);
  vk::SubmitInfo submit{};
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = &spImageAvailable;
  submit.pWaitDstStageMask = &wdsmbits;
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = &spRenderFinished;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &cb;

  m_VulkanContext->Queue().submit(submit, fence);

  vk::PresentInfoKHR present(spRenderFinished, m_Swapchain, imageIndex);
  if (m_VulkanContext->Queue().presentKHR(present) != vk::Result::eSuccess)
    return std::unexpected("Failed to present!");

  currentFrame = (currentFrame + 1) % m_FramesInFlight;

  return {};
}