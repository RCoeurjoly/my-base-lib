#pragma once

#include "ilog/ilog-impl.h"
#include "stacktrace.h"

namespace chan {
namespace ilog {
static inline void stats_sink(const std::string& base_filename, int rotation_hour, int rotation_minute, bool json_format, bool truncate, uint16_t max_files) {
  ChanLog::get().stats_sink(base_filename, rotation_hour, rotation_minute, json_format, truncate, max_files);
}
static inline void daily_file_sink(const std::string& app_name, const std::string& base_filename, int rotation_hour, int rotation_minute, bool json_format, bool truncate, uint16_t max_files) {
  ChanLog::get().daily_file_sink(app_name, base_filename, rotation_hour, rotation_minute, json_format, truncate, max_files);
}

static inline void set_enable_stdout_sink(bool enable) {
  ChanLog::get().set_enable_stdout_sink(enable);
}
static inline void set_pattern_stdout_sink(const std::string& pattern) {
  ChanLog::get().set_pattern_stdout_sink(pattern);
}
static inline void set_enable_stdout_to_json(bool enable) {
  ChanLog::get().set_enable_stdout_to_json(enable);
}

static inline void flush() {
  auto l_stats = ChanLog::stats_logger();
  if (l_stats) {
    l_stats->flush();
  }
  auto l_logger = ChanLog::stats_logger();
  if (l_logger) {
    l_logger->flush();
  }
}

static inline std::string get_filename_debug() {
  return ChanLog::get().get_filename_debug();
}

static inline std::string get_filename_info() {
  return ChanLog::get().get_filename_info();
}
static inline std::string get_filename_warn() {
  return ChanLog::get().get_filename_warn();
}
static inline std::string get_filename_error() {
  return ChanLog::get().get_filename_error();
}
static inline std::string get_filename_stats() {
  return ChanLog::get().get_filename_stats();
}

}  // namespace ilog
}  // namespace chan

#ifdef NDEBUG
#define L_DEBUG(...)
#define L_TRACE()
#define ENABLE_DEBUG()
#else
#define _L_DEBUG(l_fmt, ...) ChanLog::logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::debug, FMT_STRING(l_fmt), ##__VA_ARGS__)
#define L_DEBUG(...) _L_DEBUG(__VA_ARGS__)
#define L_TRACE() _L_DEBUG("")
#define ENABLE_DEBUG()                                         \
  ChanLog::get().logger()->set_level(spdlog::level::debug); \
  spdlog::set_level(spdlog::level::debug);
#endif

#define _L_STATS(l_fmt, ...) ChanLog::stats_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, FMT_STRING(l_fmt), ##__VA_ARGS__)
#define _L_INFO(l_fmt, ...) ChanLog::logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, FMT_STRING(l_fmt), ##__VA_ARGS__)
#define _L_WARN(l_fmt, ...) ChanLog::logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, FMT_STRING(l_fmt), ##__VA_ARGS__)
#define _L_ERROR(l_fmt, ...) ChanLog::logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, FMT_STRING(l_fmt), ##__VA_ARGS__)

#define L_INFO(...) _L_INFO(__VA_ARGS__)
#define L_WARN(...) _L_WARN(__VA_ARGS__)
#define L_ERROR(...) _L_ERROR(__VA_ARGS__)

#define _L_MAX_COUNT(l_max_repated_, _L_FUNC, ...)                                                                                                           \
  {                                                                                                                                                          \
    static int __l_count##__COUNTER__ = 0;                                                                                                                   \
    if (++__l_count##__COUNTER__ <= (l_max_repated_)) {                                                                                                      \
      _L_FUNC(__VA_ARGS__);                                                                                                                                  \
      if (__l_count##__COUNTER__ == (l_max_repated_)) {                                                                                                      \
        _L_FUNC("The log on same file and line that this message has reached the limit of repeated times: {}. Will be ommited next times.", l_max_repated_); \
      }                                                                                                                                                      \
    }                                                                                                                                                        \
  }

#define L_INFO_MAX_COUNT(l_max_repated_, ...) _L_MAX_COUNT(l_max_repated_, _L_INFO, __VA_ARGS__)
#define L_WARN_MAX_COUNT(l_max_repated_, ...) _L_MAX_COUNT(l_max_repated_, _L_WARN, __VA_ARGS__)
#define L_ERROR_MAX_COUNT(l_max_repated_, ...) _L_MAX_COUNT(l_max_repated_, _L_ERROR, __VA_ARGS__)

#define L_STATS(...) _L_STATS(__VA_ARGS__)
