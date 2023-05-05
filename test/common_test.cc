#include <catch2/catch_test_macros.hpp>

#include "Config.h"
#include "Main.h"
#include "UBX.h"

uint8_t Main_activeLED = 0;
uint8_t Main_buffer[MAIN_BUFFER_SIZE];

TEST_CASE("something", "[something]") {
    Config_Read();
    REQUIRE(UBX_nav.is_enabled());
}
