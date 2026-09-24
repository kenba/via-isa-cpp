//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Ken Barker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// @file
/// @brief Contains tests for calculate_temperature_correction_delta_altitude.
//////////////////////////////////////////////////////////////////////////////

// Altitude temoperature correction tests using values from:
// [Eurocae ED-323 Minimum Operational Performance Standards - Required
// Navigation Performance for Area
// Navigation](https://www.eurocae.net/product/ed-323-minimum-operational-performance-standards-for-required-navigation-performance-for-area-navigation/)
// Appendix H Table H-2: Altitude Correction Values

#include "via/isa.hpp"
#include <boost/test/unit_test.hpp>

using namespace via::isa;
using namespace via::units::si;
using namespace via::units::non_si;

namespace {
constinit double TOLERANCE(0.5);

constinit int COUNT{5};
constinit double TEST_ALTITUDES_FT[]{250.0, 1500.0, 2500.0, 5000.0, 10000.0};
constinit int CORRECTIONS_P25_0_FT[]{-20, -120, -201, -405, -822};
constinit int CORRECTIONS_P10_0_FT[]{-8, -51, -85, -170, -347};
constinit int CORRECTIONS_M15_0_FT[]{14, 83, 139, 280, 571};
constinit int CORRECTIONS_M30_0_FT[]{29, 175, 294, 594, 1215};
constinit int CORRECTIONS_M45_0_FT[]{46, 279 + 1, 468, 948,
                                     1947}; // 1500ft value 1 higher
constinit int CORRECTIONS_M60_0_FT[]{66, 398, 667, 1352, 2787};

constinit int CORRECTIONS_P35_5000_FT[]{-28, -168, -281, -565, -1147};
constinit int CORRECTIONS_P20_5000_FT[]{-17, -101, -168, -339, -689};
constinit int CORRECTIONS_P10_5000_FT[]{-9, -52, -87, -175,
                                        -355 - 1}; // 10000ft value 1 lower
constinit int CORRECTIONS_M5_5000_FT[]{5, 28, 47, 95, 194};
constinit int CORRECTIONS_M20_5000_FT[]{19, 118, 197, 398, 813};
constinit int CORRECTIONS_M35_5000_FT[]{36, 218, 365, 739, 1516};
constinit int CORRECTIONS_M50_5000_FT[]{55, 332, 556, 1128, 2322};

constinit int CORRECTIONS_P55_15000_FT[]{-44, -263, -440, -885, -1794};
constinit int CORRECTIONS_P40_15000_FT[]{-33, -201, -335, -676, -1371};
constinit int CORRECTIONS_P30_15000_FT[]{-26, -155, -260, -524, -1064};
constinit int CORRECTIONS_P15_15000_FT[]{-13, -81, -136, -274,
                                         -557 - 1}; // 10000ft value 1 lower
constinit int CORRECTIONS_M03_15000_FT[]{0, 2, 3, 6, 12};
constinit int CORRECTIONS_M15_15000_FT[]{16, 95, 159, 322, 658};
constinit int CORRECTIONS_M30_15000_FT[]{33, 201, 336, 681, 1398};
constinit int CORRECTIONS_M45_15000_FT[]{53, 321, 539, 1094, 2256};

template <typename T>
  requires std::floating_point<T>
void check_temperature_corrections(const Kelvin<T> delta_temperature,
                                   const Metres<T> ref_elevation,
                                   const int *corrections) {
  for (auto i = 0; i < COUNT; ++i) {
    const auto delta_altitude{Feet<T>(TEST_ALTITUDES_FT[i]).to_metres()};
    const auto result{calculate_temperature_correction_delta_altitude(
        delta_altitude + ref_elevation, delta_temperature, ref_elevation,
        Feet<T>(TOLERANCE).to_metres())};
    const auto value{Feet(std::get<0>(result)).v()};
    const auto error{std::abs(value - corrections[i])};
    BOOST_CHECK_SMALL(error, TOLERANCE);
  }
}
} // namespace

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(Test_isa_temperature_correction)

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_temperature_correction_sea_level) {
  // Reference Sea Level
  check_temperature_corrections(Kelvin(25.0), Metres(0.0),
                                CORRECTIONS_P25_0_FT);
  check_temperature_corrections(Kelvin(10.0), Metres(0.0),
                                CORRECTIONS_P10_0_FT);
  check_temperature_corrections(Kelvin(-15.0), Metres(0.0),
                                CORRECTIONS_M15_0_FT);
  check_temperature_corrections(Kelvin(-30.0), Metres(0.0),
                                CORRECTIONS_M30_0_FT);
  check_temperature_corrections(Kelvin(-45.0), Metres(0.0),
                                CORRECTIONS_M45_0_FT);
  check_temperature_corrections(Kelvin(-60.0), Metres(0.0),
                                CORRECTIONS_M60_0_FT);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_temperature_correction_from_5000ft) {
  // Reference 5000 ft
  const auto feet_5000{Feet(5000.0).to_metres()};
  check_temperature_corrections(Kelvin(34.9), feet_5000,
                                CORRECTIONS_P35_5000_FT);
  check_temperature_corrections(Kelvin(19.9), feet_5000,
                                CORRECTIONS_P20_5000_FT);
  check_temperature_corrections(Kelvin(9.9), feet_5000,
                                CORRECTIONS_P10_5000_FT);
  check_temperature_corrections(Kelvin(-5.1), feet_5000,
                                CORRECTIONS_M5_5000_FT);
  check_temperature_corrections(Kelvin(-20.1), feet_5000,
                                CORRECTIONS_M20_5000_FT);
  check_temperature_corrections(Kelvin(-35.1), feet_5000,
                                CORRECTIONS_M35_5000_FT);
  check_temperature_corrections(Kelvin(-50.1), feet_5000,
                                CORRECTIONS_M50_5000_FT);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_temperature_correction_from_15000ft) {
  // Reference 15000 ft
  const auto feet_15000{Feet(15000.0).to_metres()};
  check_temperature_corrections(Kelvin(54.7), feet_15000,
                                CORRECTIONS_P55_15000_FT);
  check_temperature_corrections(Kelvin(39.7), feet_15000,
                                CORRECTIONS_P40_15000_FT);
  check_temperature_corrections(Kelvin(29.7), feet_15000,
                                CORRECTIONS_P30_15000_FT);
  check_temperature_corrections(Kelvin(14.7), feet_15000,
                                CORRECTIONS_P15_15000_FT);
  check_temperature_corrections(Kelvin(-0.3), feet_15000,
                                CORRECTIONS_M03_15000_FT);
  check_temperature_corrections(Kelvin(-15.3), feet_15000,
                                CORRECTIONS_M15_15000_FT);
  check_temperature_corrections(Kelvin(-0.3), feet_15000,
                                CORRECTIONS_M03_15000_FT);
  check_temperature_corrections(Kelvin(-30.3), feet_15000,
                                CORRECTIONS_M30_15000_FT);
  check_temperature_corrections(Kelvin(-45.3), feet_15000,
                                CORRECTIONS_M45_15000_FT);
}
//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()
//////////////////////////////////////////////////////////////////////////////
