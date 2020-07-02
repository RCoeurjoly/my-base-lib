//#include <spdlog/spdlog.h>
#include <chrono>

#include "modern-cpp/defer.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

DOCTEST_TEST_CASE("Defer simple test") {
  auto count = 0;
  {
    DOCTEST_CHECK(count == 0);
    auto scope = sg::make_scope_guard([&count]() {
      ++count;
    });
    defer([&count]() {
      ++count;
    });
    defer([&count]() {
      ++count;
    });
    DOCTEST_CHECK(count == 0);
  }
  DOCTEST_CHECK(count == 3);
}
