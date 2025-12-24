#pragma once

#include <source_location>
#include <string>
#include <format>

namespace york {

// Non-Decided Enumeration of Errors
// Potentially will be used internally to recover from some Errors
enum class ErrorCode { Unknown = 0 };

// Error Structure with Message for direct feedback, and Location of the Error
struct Error {
  ErrorCode code = ErrorCode::Unknown;
  std::string message;
  std::source_location location;

  static Error Create(const std::string &msg, const std::source_location &loc = std::source_location::current()) {
    return {.message = msg, .location = loc};
  }

  inline std::string format() const noexcept {
    return std::format("Error at [file: {}, line: {}, function: {}]: {}",
                       location.file_name(), location.line(), location.function_name(), message);
  }
};

} // namespace york