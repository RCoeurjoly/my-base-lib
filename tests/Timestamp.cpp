#include "timestamp/timestamp.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

TEST_CASE("testing timestmap") {
  timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  chan::timestamp my_t;

  uint64_t calc_t = t.tv_sec * 1000000000 + t.tv_nsec;

  DOCTEST_CHECK(my_t.from_epoch().count() > calc_t);
  DOCTEST_CHECK(my_t.from_epoch().count() - calc_t < 100000);
  DOCTEST_SUBCASE("Checking comparison") {
    chan::timestamp my_t0;
    DOCTEST_CHECK(my_t < my_t0);
    DOCTEST_CHECK(my_t <= my_t0);
    DOCTEST_CHECK(my_t <= my_t);
    DOCTEST_CHECK(my_t0 > my_t);
    DOCTEST_CHECK(my_t0 >= my_t);
    DOCTEST_CHECK(my_t0 >= my_t0);
    DOCTEST_CHECK(my_t == my_t);
    DOCTEST_CHECK(my_t0 != my_t);
  }
  DOCTEST_SUBCASE("Check assing") {
    chan::timestamp my_t0;
    DOCTEST_CHECK(my_t < my_t0);
    my_t0 = my_t;
    DOCTEST_CHECK(my_t == my_t0);
  }

  DOCTEST_SUBCASE("Checking operations") {
    chan::timestamp my_t0;
    auto           diff = my_t0 - my_t;
    DOCTEST_CHECK(diff.count() > 0);
    chan::timestamp my_t1 = my_t0 - std::chrono::nanoseconds(1);
    auto           my_t2 = my_t0 - std::chrono::nanoseconds(2);
    DOCTEST_CHECK(my_t1 < my_t0);
    DOCTEST_CHECK(my_t1 > my_t2);

    auto my_t3 = my_t2 + std::chrono::nanoseconds(1);
    DOCTEST_CHECK(my_t3 > my_t2);
    // Not valid
    //chan::timestamp my_t3 = std::chrono::nanoseconds(1) - my_t0;
    ////chan::timestamp my_t4 = my_t0 + my_t3;
    //DOCTEST_CHECK(my_t3 < my_t0);
    ////DOCTEST_CHECK(my_t4 > my_t2);

    auto b_t2 = my_t2;
    my_t2 += std::chrono::nanoseconds(1);
    DOCTEST_CHECK(my_t2 > b_t2);
    my_t2 -= std::chrono::seconds(1);
    DOCTEST_CHECK(my_t2 < b_t2);
  }

  DOCTEST_SUBCASE("Conversion from string utc") {
    std::string    timestamp_db = "2020-03-08 14:12:09";
    chan::timestamp t0{std::chrono::seconds(1583676729)};

    chan::timestamp t1;
    t1.from_str_utc_YYYY_MM_DD_hh_mm_ss(timestamp_db);
    DOCTEST_CHECK(t0 == t1);
  }
  DOCTEST_SUBCASE("Invalid conversion from string utc") {
    std::string timestamp_db = "2020-18-08 14:12:09";

    chan::timestamp t1;
    t1.from_str_utc_YYYY_MM_DD_hh_mm_ss(timestamp_db);
    DOCTEST_CHECK(t1.is_null());
  }
};
