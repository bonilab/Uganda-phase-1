
#include "Population/Person.h"

#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("test travel tracking, person has required properties",
          "[person class]") {
#ifdef ENABLE_TRAVEL_TRACKING
  fmt::print("Travel tracking is enabled\n");
  Person person;
  REQUIRE(person.day_that_last_trip_was_initiated() == -1);
  REQUIRE(person.day_that_last_trip_outside_district_was_initiated() == -1);
#else

  fmt::print("Travel tracking is not enabled\n");
#endif
}

