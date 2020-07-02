#include <fmt/format.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include "doctest/doctest.h"

DOCTEST_TEST_CASE("testing using fmt lib") {
  std::string s = fmt::format(FMT_STRING("{}"), 42);
  DOCTEST_CHECK(s == "42");
  std::string hex = fmt::format("{:x}", 42);
  DOCTEST_CHECK(hex == "2a");
  std::string HEX = fmt::format("{:X}", 42);
  DOCTEST_CHECK(HEX == "2A");
}
