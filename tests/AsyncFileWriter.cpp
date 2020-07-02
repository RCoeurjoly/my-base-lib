// C/c++
#include <stdio.h>

// app
#include "files/AsyncFileWriter.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include "doctest/doctest.h"

DOCTEST_TEST_CASE("testing Async file writer") {
  static const std::string filename = "/tmp/test_asyn.txt";
  remove(filename.c_str());
  chan::files::async::AsyncFileWriter f("/tmp/test_asyn.txt");
  char                               test[100];
  for (size_t i = 0; i < sizeof(test); i++) {
    test[i] = i % 256;
  }
  f.write(test, sizeof(test));
  f.flush();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::ifstream result(filename, std::ios::binary | std::ios::ate);

  DOCTEST_REQUIRE(result.good());
  std::streamsize size = result.tellg();
  result.seekg(0, std::ios::beg);
  DOCTEST_REQUIRE(size == sizeof(test));
  std::vector<char> buffer(size);
  DOCTEST_REQUIRE(result.read(buffer.data(), size));
  DOCTEST_CHECK(buffer.size() == sizeof(test));
  DOCTEST_CHECK(memcmp(buffer.data(), test, sizeof(test)) == 0);
}
