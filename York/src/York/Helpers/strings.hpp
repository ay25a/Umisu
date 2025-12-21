#pragma once

#include <string>
#include <vector>

namespace york::strings {
static std::string join(const std::vector<std::string> &vec) {
  std::string res;
  for (const auto &str : vec)
    res += str + " | ";

  return std::move(res.substr(0, res.size() - 3));
}
} // namespace york::strings