#include "strings/binary.h"
#include "strings/string_algorithm.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

TEST_CASE("testing hex conversion") {
  using namespace chan::strings::bin;
  const std::string buffer_short{"hola"};
  const std::string buffer_long{"hola esto es una linea larga"};
  DOCTEST_SUBCASE("to_hex") {
    const std::string expected_short_hex{"68 6F 6C 61"};
    DOCTEST_CHECK(to_hex(reinterpret_cast<const uint8_t*>(buffer_short.c_str()), buffer_short.size()) == expected_short_hex);

    DOCTEST_CHECK(fmt::format("{:X}", HexBufferView{buffer_short}) == expected_short_hex);
    DOCTEST_CHECK(fmt::format("{:x}", HexBufferView{buffer_short}) == chan::strings::to_lower(expected_short_hex));

    const std::string expected_long_hex{"68 6F 6C 61 20 65 73 74 6F 20 65 73 20 75 6E 61 20 6C 69 6E 65 61 20 6C 61 72 67 61"};
    DOCTEST_CHECK(to_hex(reinterpret_cast<const uint8_t*>(buffer_long.c_str()), buffer_long.size()) == expected_long_hex);
    DOCTEST_CHECK(fmt::format("{:X}", HexBufferView{buffer_long}) == expected_long_hex);
    DOCTEST_CHECK(fmt::format("{:x}", HexBufferView{buffer_long}) == chan::strings::to_lower(expected_long_hex));
  };
  DOCTEST_SUBCASE("to_debug_hex") {
    const std::string expected_short_hex{
        "\n 0000. 68 6f 6c 61                    hola"};
    const std::string expected_long_hex{
        "\n 0000. 68 6f 6c 61 20 65 73 74 6f 20  hola esto "
        "\n 0010. 65 73 20 75 6e 61 20 6c 69 6e  es una lin"
        "\n 0020. 65 61 20 6c 61 72 67 61        ea larga"};
    std::string construct = "'" + to_debug_hex(reinterpret_cast<const uint8_t*>(buffer_short.c_str()), buffer_short.size()) + "'";
    std::string expected  = "'" + expected_short_hex + "'";
    DOCTEST_CHECK(construct == expected);
    DOCTEST_CHECK(to_debug_hex(reinterpret_cast<const uint8_t*>(buffer_short.c_str()), buffer_short.size()) == expected_short_hex);
    DOCTEST_CHECK(to_debug_hex(reinterpret_cast<const uint8_t*>(buffer_long.c_str()), buffer_long.size()) == expected_long_hex);

    DOCTEST_CHECK(fmt::format("'{:b}'", HexBufferView{buffer_short}) == expected);
    DOCTEST_CHECK(fmt::format("{:b}", HexBufferView{buffer_short}) == expected_short_hex);
    DOCTEST_CHECK(fmt::format("{:b}", HexBufferView{buffer_long}) == expected_long_hex);
  };
};
