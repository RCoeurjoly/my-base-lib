#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace chan {
namespace strings {
static inline void to_lower_inplace(std::string& data) {
  std::transform(data.begin(), data.end(), data.begin(),
                 [](unsigned char c) { return std::tolower(c); });
}

static inline void to_upper_inplace(std::string& data) {
  std::transform(data.begin(), data.end(), data.begin(),
                 [](unsigned char c) { return std::toupper(c); });
}

static inline void to_alphanum_inplace(std::string& data, const char print_when_not_alphanum = '.') {
  std::transform(data.begin(), data.end(), data.begin(),
                 [print_when_not_alphanum](unsigned char c) {
                   if (std::isalnum(c) || c == ' ') {
                     return c;
                   } else {
                     return static_cast<uint8_t>(print_when_not_alphanum);
                   }
                 });
}

static inline std::string to_lower(const std::string& input) {
  auto data = input;
  to_lower_inplace(data);
  return data;
}

static inline std::string to_upper(const std::string& input) {
  auto data = input;
  to_upper_inplace(data);
  return data;
}

static inline std::string to_alphanum(const std::string& input) {
  auto data = input;
  to_alphanum_inplace(data);
  return data;
}

}  // namespace strings
}  // namespace chan
