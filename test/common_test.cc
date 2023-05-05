#include <catch2/catch_test_macros.hpp>

#include "Config.h"
#include "UBX.h"

TEST_CASE("something", "[something]") {
    Config_Read();
    REQUIRE(UBX_nav.is_enabled());
}
