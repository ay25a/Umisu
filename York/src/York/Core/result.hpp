#include <expected>
#include <string>
#include <format>

namespace york {

template <typename ResultType>
using Result = std::expected<ResultType, std::string>;
using Error = Result<void>;

} // namespace york

#define YORK_SUCCESS(value) std::move(value)
#define YORK_FAILURE(fmt, ...) std::unexpected(std::format(fmt, __VA_ARGS__))

#define YORK_EXPECT(value, error) \
  if (!(value))                   \
    return std::unexpected(error);
// clang-format off
#define YORK_OK() {}