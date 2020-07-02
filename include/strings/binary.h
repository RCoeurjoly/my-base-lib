#pragma once
#include <iomanip>
#include <sstream>
#include <string>

#include <fmt/format.h>

#include "strings/string_algorithm.h"

namespace chan {
namespace strings {
namespace bin {

struct HexBufferView {
  HexBufferView(const uint8_t* p_buf, size_t p_len) : buf{const_cast<uint8_t*>(p_buf)}, len{p_len} {}
  HexBufferView(const std::string& p_buf) : buf{const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(p_buf.data()))}, len{p_buf.size()} {}
  HexBufferView(const char* p_buf, size_t p_len) : buf{const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(p_buf))}, len{p_len} {}
  uint8_t* buf = nullptr;
  size_t   len = 0;
};

// Deprectated use HexBufferView with format

static inline std::string to_hex(const uint8_t* buf, const size_t size) {
  if (size == 0) {
    return "";
  }
  std::ostringstream ret;
  for (size_t i = 0; i < size; ++i)
    ret << std::hex
        << std::setfill('0')
        << std::setw(2)
        << std::uppercase
        << ((int)(buf[i]) & 0xFF)
        << " ";
  std::string result = ret.str();
  if (!result.empty()) {
    result.pop_back();
  }
  return result;
}

// print with format:
// adddress XX XX XX (...) XX mic.fe..f.2.
static inline std::string to_debug_hex(const uint8_t* buf, size_t len) {
  if (len == 0) {
    return "";
  }
  char chars[11];
  chars[sizeof(chars) - 1] = 0;
  std::stringstream s;
  for (size_t i = 0; i < len; i++) {
    auto  offset       = i % 10;
    auto& current_char = buf[i];
    if (offset == 0) {
      if (i > 0) {
        s << "  " << chars;
      }
      s << "\n ";
      s << std::setfill('0') << std::setw(4) << std::dec << i;
      s << '.';
    }
    if (std::isalnum(current_char) || current_char == ' ') {
      chars[offset] = current_char;
    } else {
      chars[offset] = '.';
    }
    s << " ";
    s << std::setfill('0') << std::setw(2) << std::hex << static_cast<uint>(buf[i] & 0xff);
  }

  size_t offset = len % 10;
  offset        = offset == 0 ? 10 : offset;
  std::string strchars{chars, offset};
  std::string offset_space((10 - offset) * 3 + 2, ' ');
  return s.str() + offset_space + strchars;
}

};  // namespace bin
};  // namespace strings
};  // namespace chan

namespace fmt {
template <>
struct formatter<chan::strings::bin::HexBufferView> {
  //presentation is x or X
  char presentation = 'B';
  // Parses format specifications of the form ['f' | 'e'].
  auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'x' || *it == 'X' || *it == 'b' || *it == 'B')) presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the point p using the parsed format specification (presentation)
  // stored in this formatter.
  template <typename FormatContext>
  auto format(const chan::strings::bin::HexBufferView& b, FormatContext& ctx) -> decltype(format_to(ctx.out(), std::string())) {
    // ctx.out() is an output iterator to write to.
    if (presentation == 'x' || presentation == 'X') {
      return format_to(
          ctx.out(),
          (presentation == 'X') ? "{:02X}" : "{:02x}",
          fmt::join(b.buf, b.buf + b.len, " "));
    }

    constexpr size_t width = 10;
    for (size_t i = 0; i < b.len; i += width) {
      auto i_end_line = std::min(i + width, b.len);
      auto buf_start  = b.buf + i;
      auto buf_size   = i_end_line - i;

      chan::strings::bin::HexBufferView l_b(buf_start, buf_size);
      std::string                      debug_info{reinterpret_cast<char*>(buf_start), buf_size};

      const auto r = format_to(
          ctx.out(),
          (presentation == 'B') ? "\n {:04}. {:X}{:>{}}" : "\n {:04}. {:x}{:>{}}",
          i, l_b, chan::strings::to_alphanum(debug_info), 12 + ((width - buf_size) * 2));

      if (i_end_line == b.len) {
        return r;
      }
    }
    return format_to(ctx.out(), "Error formating: HexBufferView: This shouldnt be reached!!");
  }
};
}  // namespace fmt
