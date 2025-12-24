#pragma once

#include "York/Core/error.hpp"
#include <expected>

namespace york {

// Usage:
// Result<> for function with no returned data
// Result<ReturnType> for function that return a value
// In case of failure: YK_RESULT_FAILURE("<Error Message>")
// In case of success: either YK_RESULT_SUCCESS() or YK_RESULT_SUCCESS(value)

// Alias for std::expected
template <typename ReturnType = void>
using Result = std::expected<ReturnType, Error>;

} // namespace york

#define YK_RESULT_FAILURE(err) std::unexpected(err)
#define YK_RESULT_SUCCESS(...) __VA_ARGS__