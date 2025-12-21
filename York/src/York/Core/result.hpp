#pragma once

#include <expected>
#include <string>
#include <format>

namespace york {

template <typename ResultType>
using Result = std::expected<ResultType, std::string>;
using Error = Result<void>;

template <typename ResultType = void, typename... _Args>
inline Result<ResultType> failure(std::format_string<_Args...> fmt, _Args &&...args) {
  return std::unexpected(std::vformat(fmt.get(), std::make_format_args(std::forward<_Args>(args)...)));
}

template <typename ResultType>
inline Result<ResultType> ok(ResultType &res) {
  return std::move(res);
}

inline Error ok() {
  return {};
}

} // namespace york

#define YORK_EXPECT(value, error) \
  if (!(value))                   \
    return std::unexpected(error);
// clang-format off