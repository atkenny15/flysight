#include "Config.h"
#include "Main.h"
#include "UBX.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

uint8_t Main_activeLED = 0;
uint8_t Main_buffer[MAIN_BUFFER_SIZE];
FIL Main_file;

namespace {

struct Wind {
    uint32_t hMSL;
    uint32_t velE;
    uint32_t velN;
};

std::vector<Wind> get_exp_winds() {
    return std::vector<Wind>{
        Wind{
            .hMSL = 1336687,
            .velE = 9177,
            .velN = 3673,
        },

        Wind{
            .hMSL = 1643127,
            .velE = 9909,
            .velN = 2602,
        },

        Wind{
            .hMSL = 1991449,
            .velE = 10592,
            .velN = 1336,
        },

        Wind{
            .hMSL = 2386075,
            .velE = 12213,
            .velN = 790,
        },

        Wind{
            .hMSL = 2832282,
            .velE = 14124,
            .velN = 1145,
        },

        Wind{
            .hMSL = 3336756,
            .velE = 15877,
            .velN = 1725,
        },

        Wind{
            .hMSL = 3906311,
            .velE = 16723,
            .velN = 1625,
        },

        Wind{
            .hMSL = 4533619,
            .velE = 16788,
            .velN = 989,
        },
    };
}

} // namespace

TEST_CASE("Reference Point and Winds Parsing Test", "[config]") {
    constexpr double REL = 0.001;

    {
        REQUIRE(!UBX_nav.is_enabled());
        REQUIRE(!UBX_nav.is_ref_lla_valid());
    }

    { REQUIRE(UBX_num_winds == 0); }

    Config_Read();

    {
        REQUIRE(UBX_nav.is_enabled());
        REQUIRE(UBX_nav.is_ref_lla_valid());

        const auto& ref_lla = UBX_nav.get_ref_lla();
        REQUIRE_THAT(ref_lla.latitude_deg, Catch::Matchers::WithinRel(41.1458368, REL));
        REQUIRE_THAT(ref_lla.longitude_deg, Catch::Matchers::WithinRel(-80.1604920, REL));
        REQUIRE_THAT(ref_lla.height_msl_m, Catch::Matchers::WithinRel(0, REL));
    }

    {
        const auto exp_winds = get_exp_winds();
        REQUIRE(UBX_num_winds == exp_winds.size());

        for (size_t i = 0; i < exp_winds.size(); ++i) {
            REQUIRE(UBX_winds[i].hMSL == exp_winds[i].hMSL);
            REQUIRE(UBX_winds[i].velE == exp_winds[i].velE);
            REQUIRE(UBX_winds[i].velN == exp_winds[i].velN);
        }
    }
}
