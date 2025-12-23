#pragma once

#include <source_location>
#include <string>
#include <typeinfo>

namespace york {

enum class ErrorCode {
  Unknown = 0,
  WindowCreation,
  VKValidation,
  VKCreation,
};

template <typename _Type = void>
struct Error {
  ErrorCode category;
  std::string message;
  std::source_location location;

  static Error Create(ErrorCode code, const std::string &msg, const std::source_location &loc) {
    return {code, msg, loc};
  }
  const std::type_info &type() const { return typeid(_Type); }
};

} // namespace york