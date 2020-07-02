#pragma once
// c/cpp
#include <chrono>
#include <string>

//clit
#include "fmt/format.h"

#include <fmt/chrono.h>

namespace chan {

class timestamp {
 private:
  std::chrono::nanoseconds _time;

 public:
  explicit timestamp(const std::chrono::nanoseconds& time) : _time{time} {}

  timestamp() {
    static_assert(sizeof(timestamp) == sizeof(uint64_t), "Invalid size of chan::timestamp class");
    set_now();
  }

  void set_null() {
    _time = std::chrono::nanoseconds(0);
  }

  void set_now() {
    _time = now();
  }

  inline bool is_null() const {
    return _time.count() == 0;
  }

  inline std::chrono::nanoseconds from_epoch() const {
    return _time;
  };

  inline void gmtime(tm& p_tm) const {
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(from_epoch()).count();
    gmtime_r(&seconds, &p_tm);
  }

  inline tm to_gmtime() const {
    tm p_tm;
    this->gmtime(p_tm);
    return p_tm;
  }

  inline std::string to_mysql_db_utc() {
    if (is_null()) {
      return "";
    }
    return fmt::format("{:%Y-%m-%d %H:%M:%S}", to_gmtime());
  }

  inline std::string to_mysql_db_utc_us() {
    if (is_null()) {
      return "";
    }
    const std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(_time) - std::chrono::duration_cast<std::chrono::seconds>(_time);
    return fmt::format("{:%Y-%m-%d %H:%M:%S}.{:06}", to_gmtime(), us.count());
  }

  // man strptime to see format
  inline bool from_utc_strptime_fmt(const std::string& format, const std::string& time_str) {
    struct tm my_tm;
    if (strptime(time_str.c_str(), format.c_str(), &my_tm) == nullptr) {
      set_null();
      return false;
    }
    _time = std::chrono::seconds(timegm(&my_tm));
    return true;
  }

  //YYYY-MM-DD HH:MM:SS
  inline bool from_str_utc_YYYY_MM_DD_hh_mm_ss(const std::string& time_str) {
    return from_utc_strptime_fmt("%Y-%m-%d %H:%M:%S", time_str);
  }

  inline bool from_str_mysql_utc(const std::string& time_str) {
    return from_str_utc_YYYY_MM_DD_hh_mm_ss(time_str);
  }

  //YYYY-MM-DDTHH:MMM:SS.usecsZ
  inline std::string to_elastic_format() {
    if (is_null()) {
      return "";
    }
    const std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(_time) - std::chrono::duration_cast<std::chrono::seconds>(_time);
    return fmt::format("{:%Y-%m-%dT%H:%M:%S}.{:06}Z", to_gmtime(), us.count());
  }

  // comparison operators
  friend bool operator<(const timestamp& lhs, const timestamp& rhs) {
    return lhs._time < rhs._time;
  }
  friend bool operator>(const timestamp& lhs, const timestamp& rhs) {
    return lhs._time > rhs._time;
  }
  friend bool operator<=(const timestamp& lhs, const timestamp& rhs) {
    return lhs._time <= rhs._time;
  }
  friend bool operator>=(const timestamp& lhs, const timestamp& rhs) {
    return lhs._time >= rhs._time;
  }
  friend bool operator==(const timestamp& lhs, const timestamp& rhs) {
    return lhs._time == rhs._time;
  }
  friend bool operator!=(const timestamp& lhs, const timestamp& rhs) {
    return lhs._time != rhs._time;
  }
  // arithmethic
  friend std::chrono::nanoseconds operator-(const timestamp& lhs, const timestamp& rhs) {
    return lhs._time - rhs._time;
  }
  friend timestamp operator+(const timestamp& lhs, const std::chrono::nanoseconds& rhs) {
    return timestamp{lhs._time + rhs};
  }
  friend timestamp operator-(const timestamp& lhs, const std::chrono::nanoseconds& rhs) {
    return timestamp{lhs._time - rhs};
  }

  timestamp& operator+=(const std::chrono::nanoseconds& rhs) {
    this->_time += rhs;
    return *this;
  }

  timestamp& operator-=(const std::chrono::nanoseconds& rhs) {
    this->_time -= rhs;
    return *this;
  }

  static std::chrono::nanoseconds now() {
    timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return std::chrono::seconds(t.tv_sec) + std::chrono::nanoseconds(t.tv_nsec);
  }

  friend std::ostream& operator<<(std::ostream& stream, const timestamp& t) {
    stream << fmt::format("{:s}", t);
    return stream;
  }
};
};  // namespace chan

namespace fmt {
template <>
struct formatter<chan::timestamp> {
  //presentation is s or d
  char presentation = 's';
  // Parses format specifications of the form ['f' | 'e'].
  auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 's' || *it == 'd')) presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the point p using the parsed format specification (presentation)
  // stored in this formatter.
  template <typename FormatContext>
  auto format(const chan::timestamp& t, FormatContext& ctx) -> decltype(format_to(ctx.out(), std::string())) {
    // ctx.out() is an output iterator to write to.
    if (presentation == 'd') {
      return format_to(ctx.out(), "{}", t.from_epoch().count());
    } else {
      const auto nano_s = t.from_epoch() - std::chrono::duration_cast<std::chrono::seconds>(t.from_epoch());
      return fmt::format_to(ctx.out(), "{:%Y-%m-%d %H:%M:%S}.{:09}", t.to_gmtime(), nano_s.count());
    }
  }  // namespace fmt
};
};  // namespace fmt
