#pragma once

#include <expected>
#include <format>
#include <source_location>
#include "York/Core/error.hpp"

namespace york {

template <typename ResultType, typename ErrorType>
using Result = std::expected<ResultType, Error<ErrorType>>;

using Status = std::expected<void, std::string>;

template <typename... Args>
inline Status failure(std::format_string<Args...> fmt, Args &&...args) {
  return std::unexpected(std::vformat(fmt.get(), std::make_format_args(args...)));
};

template <typename ResultType, typename ErrorType>
inline Result<ResultType, ErrorType> failure(const Status &status, ErrorCode code, const std::source_location &location = std::source_location::current()) {
  return std::unexpected(Error<ErrorType>::Create(code, status.error(), location));
}

template <typename ResultType, typename ErrorType>
inline Result<ResultType, ErrorType> failure(const std::string &msg, ErrorCode code, const std::source_location &location = std::source_location::current()) {
  return std::unexpected(Error<ErrorType>::Create(code, msg, location));
}

// clang-format off
#define STATUS_SUCCESS {}
// clang-format on

} // namespace york