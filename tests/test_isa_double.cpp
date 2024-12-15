//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Ken Barker
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
/// @brief Contains tests for the via::isa namespace.
//////////////////////////////////////////////////////////////////////////////
#include "via/isa.hpp"
#include <boost/test/unit_test.hpp>

using namespace via::isa;
using namespace via::units::si;
using namespace via::units::non_si;

namespace {
const double CALCULATION_TOLERANCE(1.0e-6);
} // namespace

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(Test_isa_double)

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_calculate_isa_pressure) {
  // calculate_troposphere_pressure
  Pascals<double> result(calculate_isa_pressure(Metres<double>(0.0)));
  BOOST_CHECK_EQUAL(constants::SEA_LEVEL_PRESSURE<double>.v(), result.v());

  result = calculate_isa_pressure(Metres<double>(1000.0));
  BOOST_CHECK_CLOSE(89874.563, result.v(), CALCULATION_TOLERANCE);

  result = calculate_isa_pressure(Metres<double>(2000.0));
  BOOST_CHECK_CLOSE(79495.202, result.v(), CALCULATION_TOLERANCE);

  result = calculate_isa_pressure(Metres<double>(10999.0));
  BOOST_CHECK_CLOSE(22635.609, result.v(), CALCULATION_TOLERANCE);

  // calculate_tropopause_pressure
  Pascals<double> troposphere_pressure{
      calculate_troposphere_pressure(constants::TROPOPAUSE_ALTITUDE<double>)};
  Pascals<double> tropopause_pressure{
      calculate_tropopause_pressure(constants::TROPOPAUSE_ALTITUDE<double>)};
  BOOST_CHECK_CLOSE(tropopause_pressure.v(), troposphere_pressure.v(),
                    CALCULATION_TOLERANCE);

  result = calculate_isa_pressure(constants::TROPOPAUSE_ALTITUDE<double>);
  BOOST_CHECK_EQUAL(TROPOPAUSE_PRESSURE<double>.v(), result.v());

  result = calculate_isa_pressure(Metres<double>(12000.0));
  BOOST_CHECK_CLOSE(19330.3825, result.v(), CALCULATION_TOLERANCE);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_calculate_isa_altitude) {
  // calculate_troposphere_altitude
  Metres<double> result(
      calculate_isa_altitude(constants::SEA_LEVEL_PRESSURE<double>));
  BOOST_CHECK_EQUAL(0.0, result.v());

  result = calculate_isa_altitude(Pascals<double>(105000.0)); // 1050mB
  BOOST_CHECK_CLOSE(-301.51854804303838, result.v(), CALCULATION_TOLERANCE);

  result = calculate_isa_altitude(
      Pascals<double>(constants::SEA_LEVEL_PRESSURE<double>.v() - 10000.0));
  BOOST_CHECK_CLOSE(867.8115222838419, result.v(), CALCULATION_TOLERANCE);

  result = calculate_isa_altitude(Pascals<double>(89874.563));
  BOOST_CHECK_CLOSE(1000.0, result.v(), CALCULATION_TOLERANCE);

  result = calculate_isa_altitude(Pascals<double>(79495.202));
  BOOST_CHECK_CLOSE(2000.0, result.v(), CALCULATION_TOLERANCE);

  result = calculate_isa_altitude(Pascals<double>(60000.0)); // 600mB
  BOOST_CHECK_CLOSE(4206.4224277251433, result.v(), CALCULATION_TOLERANCE);

  result = calculate_isa_altitude(Pascals<double>(22635.609));
  BOOST_CHECK_CLOSE(10999.0, result.v(), CALCULATION_TOLERANCE);

  // calculate_tropopause_altitude
  result = calculate_isa_altitude(TROPOPAUSE_PRESSURE<double>);
  BOOST_CHECK_CLOSE(constants::TROPOPAUSE_ALTITUDE<double>.v(), result.v(),
                    CALCULATION_TOLERANCE);

  result = calculate_isa_altitude(Pascals<double>(19330.3825));
  BOOST_CHECK_CLOSE(12000.0, result.v(), CALCULATION_TOLERANCE);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_calculate_isa_temperature) {
  Kelvin<double> result{calculate_isa_temperature(Metres<double>(0.0))};
  BOOST_CHECK_EQUAL(constants::SEA_LEVEL_TEMPERATURE<double>.v(), result.v());

  result = calculate_isa_temperature(Metres<double>(500.0));
  BOOST_CHECK_EQUAL(constants::SEA_LEVEL_TEMPERATURE<double>.v() - 3.25,
                    result.v());

  result = calculate_isa_temperature(Metres<double>(2000.0));
  BOOST_CHECK_EQUAL(constants::SEA_LEVEL_TEMPERATURE<double>.v() - 13.0,
                    result.v());

  result = calculate_isa_temperature(constants::TROPOPAUSE_ALTITUDE<double>);
  BOOST_CHECK_EQUAL(constants::TROPOPAUSE_TEMPERATURE<double>.v(), result.v());

  result = calculate_isa_temperature(Metres<double>(12000.0));
  BOOST_CHECK_EQUAL(constants::TROPOPAUSE_TEMPERATURE<double>.v(), result.v());
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_calculate_density) {
  KilogramsPerCubicMetre<double> result =
      calculate_density(constants::SEA_LEVEL_PRESSURE<double>,
                        constants::SEA_LEVEL_TEMPERATURE<double>);
  BOOST_CHECK_CLOSE(constants::SEA_LEVEL_DENSITY<double>.v(), result.v(),
                    2 * CALCULATION_TOLERANCE);
  // result is 1.2250000181242879

  // Tropopause
  result = calculate_density(TROPOPAUSE_PRESSURE<double>,
                             constants::TROPOPAUSE_TEMPERATURE<double>);
  BOOST_CHECK_CLOSE(0.36391765, result.v(), CALCULATION_TOLERANCE);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_calculate_true_air_speed) {
  MetresPerSecond<double> result = calculate_true_air_speed(
      MetresPerSecond<double>(150.0), constants::SEA_LEVEL_PRESSURE<double>,
      constants::SEA_LEVEL_TEMPERATURE<double>);
  BOOST_CHECK_CLOSE(150.0, result.v(), CALCULATION_TOLERANCE);

  // Calculate TAS at 2000 metres
  result = calculate_true_air_speed(
      MetresPerSecond<double>(150.0), Pascals<double>(79495.202),
      Kelvin<double>(constants::SEA_LEVEL_TEMPERATURE<double>.v() - 13.0));
  BOOST_CHECK_CLOSE(164.457894, result.v(), CALCULATION_TOLERANCE);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_calculate_calibrated_air_speed) {
  MetresPerSecond<double> result = calculate_calibrated_air_speed(
      MetresPerSecond<double>(150.0), constants::SEA_LEVEL_PRESSURE<double>,
      constants::SEA_LEVEL_TEMPERATURE<double>);
  BOOST_CHECK_CLOSE(150.0, result.v(), CALCULATION_TOLERANCE);

  // Calculate CAS at 2000 metres
  result = calculate_calibrated_air_speed(
      MetresPerSecond<double>(164.457894), Pascals<double>(79495.202),
      Kelvin<double>(constants::SEA_LEVEL_TEMPERATURE<double>.v() - 13.0));
  BOOST_CHECK_CLOSE(150.0, result.v(), CALCULATION_TOLERANCE);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_speed_of_sound) {
  MetresPerSecond<double> result =
      speed_of_sound(constants::SEA_LEVEL_TEMPERATURE<double>);
  BOOST_CHECK_CLOSE(constants::SEA_LEVEL_SPEED_OF_SOUND<double>.v(), result.v(),
                    10 * CALCULATION_TOLERANCE);
  result = speed_of_sound(constants::TROPOPAUSE_TEMPERATURE<double>);
  BOOST_CHECK_CLOSE(295.069493, result.v(), CALCULATION_TOLERANCE);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_mach_true_air_speed) {
  MetresPerSecond<double> result =
      mach_true_air_speed(0.8, constants::SEA_LEVEL_TEMPERATURE<double>);
  BOOST_CHECK_CLOSE(0.8 * constants::SEA_LEVEL_SPEED_OF_SOUND<double>.v(),
                    result.v(), 10 * CALCULATION_TOLERANCE);
  result = mach_true_air_speed(0.85, constants::TROPOPAUSE_TEMPERATURE<double>);
  BOOST_CHECK_CLOSE(0.85 * 295.069493, result.v(), CALCULATION_TOLERANCE);
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(test_calculate_crossover_altitude) {
  auto cas = MetresPerSecond<double>(155.0);
  double mach = 0.79;
  Metres<double> crossover_altitude = calculate_crossover_altitude(cas, mach);
  BOOST_CHECK_CLOSE(9070.813566, crossover_altitude.v(), CALCULATION_TOLERANCE);

  // The TAS should be the same from both CAS and MACH at the crossover_altitude
  Pascals<double> pressure = calculate_isa_pressure(crossover_altitude);
  Kelvin<double> temperature = calculate_isa_temperature(crossover_altitude);
  MetresPerSecond<double> tas_from_cas =
      calculate_true_air_speed(cas, pressure, temperature);
  MetresPerSecond<double> tas_from_mach =
      mach_true_air_speed(0.79, temperature);
  BOOST_CHECK_CLOSE(tas_from_cas.v(), tas_from_mach.v(),
                    4 * CALCULATION_TOLERANCE);
  BOOST_CHECK_CLOSE(239.75607, tas_from_mach.v(), CALCULATION_TOLERANCE);
}
//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()
//////////////////////////////////////////////////////////////////////////////
