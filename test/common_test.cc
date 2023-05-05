#include <catch2/catch_test_macros.hpp>

#include "Config.h"

TEST_CASE("something", "[something]") {
    Config_Read();
    REQUIRE();
}
