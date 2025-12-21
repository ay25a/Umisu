#pragma once

#include <spdlog/spdlog.h>
#include <memory>
namespace york {
class Logger {
public:
  static std::shared_ptr<spdlog::logger> s_EngineLogger;
  static std::shared_ptr<spdlog::logger> s_VulkanLogger;
  static std::shared_ptr<spdlog::logger> s_RuntimeLogger;

public:
  static void init();
};
} // namespace york

#define YORK_ENGINE_LOG_TRACE(...) ::york::Logger::s_EngineLogger->trace(__VA_ARGS__)
#define YORK_ENGINE_LOG_INFO(...) ::york::Logger::s_EngineLogger->info(__VA_ARGS__)
#define YORK_ENGINE_LOG_WARN(...) ::york::Logger::s_EngineLogger->warn(__VA_ARGS__)
#define YORK_ENGINE_LOG_ERROR(...) ::york::Logger::s_EngineLogger->error(__VA_ARGS__)
#define YORK_ENGINE_LOG_CRITICAL(...) ::york::Logger::s_EngineLogger->critical(__VA_ARGS__)

#define YORK_VULKAN_LOG_TRACE(...) ::york::Logger::s_VulkanLogger->trace(__VA_ARGS__)
#define YORK_VULKAN_LOG_INFO(...) ::york::Logger::s_VulkanLogger->info(__VA_ARGS__)
#define YORK_VULKAN_LOG_WARN(...) ::york::Logger::s_VulkanLogger->warn(__VA_ARGS__)
#define YORK_VULKAN_LOG_ERROR(...) ::york::Logger::s_VulkanLogger->error(__VA_ARGS__)
#define YORK_VULKAN_LOG_CRITICAL(...) ::york::Logger::s_VulkanLogger->critical(__VA_ARGS__)

#define YORK_RUNTIME_LOG_TRACE(...) ::york::Logger::s_RuntimeLogger->trace(__VA_ARGS__)
#define YORK_RUNTIME_LOG_INFO(...) ::york::Logger::s_RuntimeLogger->info(__VA_ARGS__)
#define YORK_RUNTIME_LOG_WARN(...) ::york::Logger::s_RuntimeLogger->warn(__VA_ARGS__)
#define YORK_RUNTIME_LOG_ERROR(...) ::york::Logger::s_RuntimeLogger->error(__VA_ARGS__)
#define YORK_RUNTIME_LOG_CRITICAL(...) ::york::Logger::s_RuntimeLogger->critical(__VA_ARGS__)