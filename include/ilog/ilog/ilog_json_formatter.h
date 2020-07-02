#pragma once

#include <spdlog/details/fmt_helper.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

class JsonFormatter final : public spdlog::formatter {
 private:
  std::string _app_name;

 public:
  explicit JsonFormatter(){};

  // use default pattern is not given
  ///explicit JsonFormatter();

  JsonFormatter(const JsonFormatter& other) = delete;
  JsonFormatter& operator=(const JsonFormatter& other) = delete;

  std::unique_ptr<formatter> clone() const override {
    return spdlog::details::make_unique<JsonFormatter>();
  };

  void set_app_name(const std::string& s) {
    _app_name = s;
  }

  void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override {
    nlohmann::json j;

    struct tm _tm;
    time_t    t = std::chrono::duration_cast<std::chrono::seconds>(msg.time.time_since_epoch()).count();

    gmtime_r(&t, &_tm);
    auto micros = spdlog::details::fmt_helper::time_fraction<std::chrono::microseconds>(msg.time).count();

    j["log"]["level"]                  = spdlog::level::to_string_view(msg.level).data();
    j["@timestamp"]                    = fmt::format("{}-{:#02}-{:#02}T{:#02}:{:#02}:{:#02}.{:#06}Z", _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec, micros);
    j["log"]["thread_id"]              = fmt::format("{}", msg.thread_id);
    j["log"]["origin"]["file"]["name"] = basename(msg.source.filename);
    j["log"]["origin"]["file"]["line"] = msg.source.line;
    j["message"]                       = std::string(msg.payload.data(), msg.payload.size());
    if (_app_name.size())
      j["labels"]["service"] = _app_name;

    std::string data = j.dump() + '\n';
    dest.append(data.data(), data.data() + data.size());
  };
};
