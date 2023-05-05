#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Config.h"
#include "Main.h"
#include "UBX.h"

uint8_t Main_activeLED = 0;
uint8_t Main_buffer[MAIN_BUFFER_SIZE];
FIL     Main_file;

TEST_CASE("something", "[something]") {
    constexpr double REL = 0.001;

    REQUIRE(!UBX_nav.is_enabled());
    REQUIRE(!UBX_nav.is_ref_lla_valid());

    Config_Read();
    REQUIRE(UBX_nav.is_enabled());
    REQUIRE(UBX_nav.is_ref_lla_valid());

    const auto &ref_lla = UBX_nav.get_ref_lla();
    REQUIRE_THAT(ref_lla.latitude_deg, Catch::Matchers::WithinRel(41.1458368, REL));
    REQUIRE_THAT(ref_lla.longitude_deg, Catch::Matchers::WithinRel(-80.1604920, REL));
    REQUIRE_THAT(ref_lla.height_msl_m, Catch::Matchers::WithinRel(0, REL));
}
