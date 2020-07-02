#pragma once
//#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
// clang-format off
//spdlog
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
// clang-format on

#include "ilog_json_formatter.h"

class ChanLog {
 private:
  std::shared_ptr<spdlog::async_logger> _stats_logger;

  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> _stdout_sink;

  std::shared_ptr<spdlog::sinks::dist_sink_mt> _dist_sink;

  std::shared_ptr<spdlog::sinks::daily_file_sink_mt> _f_debug_sink;
  std::shared_ptr<spdlog::sinks::daily_file_sink_mt> _f_info_sink;
  std::shared_ptr<spdlog::sinks::daily_file_sink_mt> _f_warn_sink;
  std::shared_ptr<spdlog::sinks::daily_file_sink_mt> _f_error_sink;

  const std::string _text_pattern{"%L%d%m %T.%f %t %s:%#] %v"};
  const std::string _json_pattern{
      "{"
      "\"level\":\"%L\","
      "\"datetime\":\"%Y-%m-%d %T.%f\","
      "\"th_id\":%t,"
      "\"loc\":\"%s:%#\","
      "\"log\":\"%v\""
      "},"};
  const std::string cout_pattern{"%^" + _text_pattern + "%$"};

  const spdlog::level::level_enum default_level = spdlog::level::info;
  inline spdlog::logger*          get_default_logger() {
    return spdlog::default_logger_raw();
  }

  inline void set_file_to_dist_sink(std::shared_ptr<spdlog::sinks::daily_file_sink_mt>& f_sink,
                                    const std::string&                                  app_name,
                                    const std::string                                   filename,
                                    const spdlog::level::level_enum                     level,
                                    bool                                                json,
                                    int                                                 rotation_hour,
                                    int                                                 rotation_minute,
                                    bool                                                truncate,
                                    int                                                 max_files) {
    if (f_sink) {
      _dist_sink->remove_sink(f_sink);
    }
    f_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(filename, rotation_hour, rotation_minute, truncate, max_files);
    f_sink->set_level(level);
    set_enable_to_json(f_sink, json);
    _dist_sink->add_sink(f_sink);
  }

  inline void set_file_to_dist_sink(std::shared_ptr<spdlog::sinks::daily_file_sink_mt>& f_sink,
                                    const std::string                                   filename,
                                    const spdlog::level::level_enum                     level,
                                    const std::string&                                  pattern,
                                    int                                                 rotation_hour,
                                    int                                                 rotation_minute,
                                    bool                                                truncate,
                                    int                                                 max_files) {
    if (f_sink) {
      _dist_sink->remove_sink(f_sink);
    }
    f_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(filename, rotation_hour, rotation_minute, truncate, max_files);
    f_sink->set_level(level);
    f_sink->set_pattern(pattern);
    _dist_sink->add_sink(f_sink);
  }
  static inline std::string get_file_from_sink(const std::shared_ptr<spdlog::sinks::daily_file_sink_mt>& f_sink) {
    if (f_sink) {
      return f_sink->filename();
    }
    return "";
  }
  void flush() {
    if (_stats_logger) {
      _stats_logger->flush();
    }

    auto l_logger = get_default_logger();
    if (l_logger) {
      l_logger->flush();
    }

    if (_dist_sink) {
      _dist_sink->flush();
    }

    if (_f_error_sink) {
      _f_error_sink->flush();
    }
    if (_f_warn_sink) {
      _f_warn_sink->flush();
    }
    if (_f_info_sink) {
      _f_info_sink->flush();
    }
    if (_f_debug_sink) {
      _f_debug_sink->flush();
    }
  }

 public:
  ChanLog() {
    //std::cout << "Construyendo ChanLog\n\n";
    spdlog::set_level(default_level);
    spdlog::set_pattern(_text_pattern, spdlog::pattern_time_type::utc);
    // Asi se evita problemas de desordenamiento
    spdlog::init_thread_pool(8192 * 2, 1);
    //spdlog::init_thread_pool(8192 * 2, 2);
    spdlog::flush_every(std::chrono::seconds(5));

    _dist_sink   = std::make_shared<spdlog::sinks::dist_sink_mt>();
    _stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    _dist_sink->add_sink(_stdout_sink);

    auto logger = std::make_shared<spdlog::async_logger>("G", _dist_sink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);

    logger->set_pattern(_text_pattern, spdlog::pattern_time_type::utc);
    logger->set_level(default_level);
    _stdout_sink->set_pattern(cout_pattern);
    spdlog::set_default_logger(logger);
  }
  ~ChanLog() {
    flush();
  }

  void set_pattern_stdout_sink(const std::string& pattern) {
    _stdout_sink->set_pattern(pattern);
  }

  void set_enable_stdout_sink(bool enable) {
    if (enable) {
      _dist_sink->add_sink(_stdout_sink);
    } else {
      _dist_sink->remove_sink(_stdout_sink);
    }
  }

  template <typename SINK>
  void set_enable_to_json(SINK& s, bool enable) {
    if (enable) {
      auto json_formatter = spdlog::details::make_unique<JsonFormatter>();
      s->set_formatter(std::move(json_formatter));
    } else {
      // lo copiamos porque sera movido en el interior
      std::string text_pattern{_text_pattern};
      s->set_pattern(text_pattern);
    }
  }

  void set_enable_stdout_to_json(bool enable) {
    set_enable_to_json(_stdout_sink, enable);
  }

  void daily_file_sink(const std::string& app_name, const std::string& base_filename, int rotation_hour, int rotation_minute, bool json_format, bool truncate, uint16_t max_files) {
    //const auto& pattern = json_format ? _json_pattern : _text_pattern;
    //set_file_to_dist_sink(_f_debug_sink, base_filename + ".DEBUG", spdlog::level::debug, pattern, rotation_hour, rotation_minute, truncate, max_files);
    if (spdlog::default_logger()->level() == spdlog::level::debug) {
      set_file_to_dist_sink(_f_debug_sink, app_name, base_filename + "_spdl.DEBUG", spdlog::level::debug, json_format, rotation_hour, rotation_minute, truncate, max_files);
    }

    set_file_to_dist_sink(_f_info_sink, app_name, base_filename + "_spdl.INFO", spdlog::level::info, json_format, rotation_hour, rotation_minute, truncate, max_files);
    set_file_to_dist_sink(_f_warn_sink, app_name, base_filename + "_spdl.WARN", spdlog::level::warn, json_format, rotation_hour, rotation_minute, truncate, max_files);
    set_file_to_dist_sink(_f_error_sink, app_name, base_filename + "_spdl.ERROR", spdlog::level::err, json_format, rotation_hour, rotation_minute, truncate, max_files);

    //set_file_to_dist_sink(_f_info_sink, base_filename + ".INFO", spdlog::level::info, pattern, rotation_hour, rotation_minute, truncate, max_files);
    //set_file_to_dist_sink(_f_warn_sink, base_filename + ".WARN", spdlog::level::warn, pattern, rotation_hour, rotation_minute, truncate, max_files);
    //set_file_to_dist_sink(_f_error_sink, base_filename + ".ERROR", spdlog::level::err, pattern, rotation_hour, rotation_minute, truncate, max_files);
  }

  void stats_sink(const std::string& base_filename, int rotation_hour, int rotation_minute, bool json_format, bool truncate, uint16_t max_files) {
    static const std::string stats_id{"STATS"};

    if (spdlog::get(stats_id)) {
      spdlog::drop(stats_id);
    }
    auto stats_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(base_filename + "_spdl.stats", rotation_hour, rotation_minute, truncate, max_files);
    _stats_logger   = std::make_shared<spdlog::async_logger>(stats_id, stats_sink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    _stats_logger->set_level(spdlog::level::info);
    _stats_logger->set_pattern("%v", spdlog::pattern_time_type::utc);
    spdlog::register_logger(_stats_logger);
  }

  static inline ChanLog& get() {
    static ChanLog _logger;
    return _logger;
  }

  std::string get_filename_debug() const {
    return get_file_from_sink(_f_debug_sink);
  }

  std::string get_filename_info() const {
    return get_file_from_sink(_f_info_sink);
  }

  std::string get_filename_warn() const {
    return get_file_from_sink(_f_warn_sink);
  }

  std::string get_filename_error() const {
    return get_file_from_sink(_f_error_sink);
  }

  std::string get_filename_stats() const {
    if (!_stats_logger) {
      return "";
    }
    const auto& sinks = _stats_logger->sinks();
    if (sinks.size() == 0) {
      return "";
    }

    const auto& sink = sinks.back();
    if (!sink) {
      return "";
    }
    auto f_sink = dynamic_cast<spdlog::sinks::daily_file_sink_mt*>(sink.get());
    return f_sink->filename();
  }

  static inline spdlog::logger* logger() {
    return get().get_default_logger();
  }

  static inline spdlog::logger* stats_logger() {
    return get()._stats_logger.get();
  }
};

// Compablity layer with glog

class StreamDelegate {
 public:
  StreamDelegate(std::stringstream& os, spdlog::level::level_enum level, const char* file, const char* function, const int line)
      : os(os), level(level), file(file), function(function), line(line) {}

  //logger->log(spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)
  ~StreamDelegate() {
    ChanLog::logger()->log(spdlog::source_loc{file, line, function}, level, os.str());
    os.str(std::string());
  }
  template <class T>
  StreamDelegate& operator<<(T&& output) {
    os << std::forward<T>(output);
    return *this;
  }

  StreamDelegate(const StreamDelegate&) = delete;
  StreamDelegate& operator=(const StreamDelegate&) = delete;

  StreamDelegate(StreamDelegate&&) = default;
  StreamDelegate& operator=(StreamDelegate&&) = default;

 private:
  std::stringstream&        os;
  spdlog::level::level_enum level;
  const char*               file;
  const char*               function;
  const int                 line;
};

/////////////////////////////////////////////////////////////////////
class CHANLogger {
 private:
  std::stringstream _out_stream;

 public:
  template <typename T>
  CHANLogger& operator<<(T const& value) {
    //log.your_stringstream << value;
    _out_stream << value;
    //spdlog::info("{}", value);
    return *this;
  }
  class StreamDelegate log(spdlog::level::level_enum level, const char* file, const char* function, const int line) {
    return StreamDelegate(_out_stream, level, file, function, line);
  }
  static inline CHANLogger& get() {
    static CHANLogger my_logger;
    return my_logger;
  }
};

//extern CHANLogger MyLogger;

//TODO crear otro StreamDelegate que use el logger para syslog en vez de el default

#define LOG_IMP_INFO CHANLogger::get().log(spdlog::level::info, __FILE__, SPDLOG_FUNCTION, __LINE__)
#define LOG_IMP_WARNING CHANLogger::get().log(spdlog::level::warn, __FILE__, SPDLOG_FUNCTION, __LINE__)
#define LOG_IMP_ERROR CHANLogger::get().log(spdlog::level::err, __FILE__, SPDLOG_FUNCTION, __LINE__)

#define LOG(LEVEL) LOG_IMP_##LEVEL
#define SYSLOG(LEVEL) LOG_IMP_##LEVEL
