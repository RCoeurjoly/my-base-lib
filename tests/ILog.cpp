//#include <spdlog/spdlog.h>
#include <chrono>

#include "ilog/ilog.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

DOCTEST_TEST_CASE("logger") {
  ENABLE_DEBUG();
  // L_DEBUG("hola debug {}", 5);
  // L_INFO("hola {}", "info", 5);
  for (int i = 0; i < 10; i++) {
    L_INFO("testing normal before max_count. i = {}", i);
    L_INFO_MAX_COUNT(3, "testing max_count i = {}", i);
    L_WARN_MAX_COUNT(5, "testing max_count i = {}", i);
    L_ERROR_MAX_COUNT(7, "testing max_count i = {}", i);
    L_INFO("testing normal after max_count. i = {}", i);
  }
  chan::ilog::flush();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  chan::ilog::flush();
  LOG(INFO) << "Testing info file before: " << chan::ilog::get_filename_info();
  LOG(INFO) << "Testing info file before: " << chan::ilog::get_filename_stats();
  LOG(INFO) << "Testing compatibility: " << 1;
  LOG(WARNING) << "Testing compatibility: " << 2;
  LOG(ERROR) << "Testing compatibility: " << 5;
  chan::ilog::set_enable_stdout_to_json(true);
  chan::ilog::flush();
  L_INFO("hola {} {}", "info", 5);
  L_DEBUG("hola debug {}", 5);
  L_WARN("hola {} {}", "W", 5);
  L_ERROR("hola {} {}", "ERR", 5);
  L_DEBUG("back new:\n{}", ilog::stacktrace());
  L_ERROR("Solo mensaje de error");
  L_TRACE();

  auto   now    = std::chrono::system_clock::now() + std::chrono::minutes(1);
  time_t tt     = std::chrono::system_clock::to_time_t(now);
  tm     utc_tm = *gmtime(&tt);

  int rotation_hour   = utc_tm.tm_hour;
  int rotation_minute = utc_tm.tm_min;

  chan::ilog::daily_file_sink("test_app", "test", rotation_hour, rotation_minute, true, false, 0);
  chan::ilog::stats_sink("test", rotation_hour, rotation_minute, true, false, 0);

  chan::ilog::set_enable_stdout_to_json(false);
  chan::ilog::flush();

  bool       enable = false;
  const auto max    = 20;
  for (int i = 0; i < 20; i++) {
    LOG(INFO) << "Testing info file after: " << chan::ilog::get_filename_info();
    LOG(INFO) << "Testing info file before: " << chan::ilog::get_filename_stats();
    L_DEBUG("hola debug {}", 5);
    L_INFO("hola {} {}", "info", 5);
    L_WARN("hola {} {}", "W", 5);
    L_ERROR("hola {} {}", "ERR", 5);
    L_STATS("{{ \"test\": {} }}", i);
    LOG(INFO) << "Value of : " << i;
    LOG(WARNING) << "0 Testing compatibility:\" with quotes \" and : " << 2;

    LOG(ERROR) << "0 Testing compatibility: " << 5;
    if (i < (max - 1)) {
      chan::ilog::set_enable_stdout_sink(enable);
      enable = !enable;
      std::this_thread::sleep_for(std::chrono::milliseconds(90));
    }
  }
  chan::ilog::flush();
}
