#include "logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace york {
std::shared_ptr<spdlog::logger> Logger::s_EngineLogger;
std::shared_ptr<spdlog::logger> Logger::s_VulkanLogger;
std::shared_ptr<spdlog::logger> Logger::s_RuntimeLogger;

void Logger::init() {
  spdlog::set_pattern("%^[%T] %n: %v%$");
  s_EngineLogger = spdlog::stdout_color_mt("ENGINE");
  s_EngineLogger->set_level(spdlog::level::trace);

  s_VulkanLogger = spdlog::stdout_color_mt("VULKAN");
  s_VulkanLogger->set_level(spdlog::level::trace);

  s_RuntimeLogger = spdlog::stdout_color_mt("RUNTIME");
  s_RuntimeLogger->set_level(spdlog::level::trace);
}
} // namespace york