#pragma once

#include <expected>
#include <string>
#include <format>
#include <typeinfo>

namespace york {
enum class ErrorCategory {
  None = 0,
  Creation,
  Vulkan,
};

struct Error {
  ErrorCategory Category;
  const std::type_info &TypeInfo;
  std::string Reason;
};

template <typename ResultType = void>
using Result = std::expected<ResultType, Error>;

template <typename ResultType = void>
inline constexpr Result<ResultType> failure(const Error &error) { return std::unexpected(error); }

inline constexpr Result<void> failure(std::string_view reason) { return std::unexpected(Error{ErrorCategory::None, typeid(void), reason.data()}); }

template <typename ResultType>
inline constexpr Result<ResultType> failure(const Result<> &inner, ErrorCategory category) {
  return std::unexpected(Error{category, typeid(ResultType), inner.error().Reason});
}

template <typename ResultType>
inline constexpr Result<ResultType> failure(std::string_view reason, ErrorCategory category) {
  return std::unexpected(Error{category, typeid(ResultType), reason.data()});
}

template <typename ResultType>
inline Result<ResultType> ok(ResultType &res) {
  return std::move(res);
}

inline Result<> ok() {
  return {};
}

template <typename ResultType = void>
inline bool has_failed(const Result<ResultType> &res) noexcept { return !res.has_value(); }
} // namespace york

#define YORK_EXPECT(value, error) \
  if (!(value))                   \
    return std::unexpected(error);

// clang-format off