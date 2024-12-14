#!/usr/bin/env python

# Copyright (c) 2024 Ken Barker
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#  @file test_units
#  @brief Contains unit tests for the via units classes and constants.

import pytest
from numpy.testing import assert_almost_equal
from via_units import Kelvin, KilogramsPerCubicMetre, Metres, MetresPerSecond, Pascals
from via_isa import calculate_density, calculate_isa_altitude, calculate_isa_pressure, \
calculate_isa_temperature, calculate_calibrated_air_speed, calculate_true_air_speed, \
mach_true_air_speed, speed_of_sound, calculate_crossover_altitude, \
SEA_LEVEL_DENSITY, SEA_LEVEL_PRESSURE, SEA_LEVEL_TEMPERATURE, SEA_LEVEL_SPEED_OF_SOUND, \
TROPOPAUSE_ALTITUDE, TROPOPAUSE_TEMPERATURE

def test_calculate_isa_pressure():
    # calculate_troposphere_pressure
    result = calculate_isa_pressure(Metres(0.0))
    assert SEA_LEVEL_PRESSURE.v() == result.v()

    result = calculate_isa_pressure(Metres(1000.0))
    assert_almost_equal(89874.56291622, result.v())

    result = calculate_isa_pressure(Metres(2000.0))
    assert_almost_equal(79495.20193405, result.v())

    result = calculate_isa_pressure(Metres(10999.0))
    assert_almost_equal(22635.60913586, result.v())

    # calculate_tropopause_pressure
    result = calculate_isa_pressure(TROPOPAUSE_ALTITUDE)
    assert_almost_equal(22_632.040_095_007_81, result.v())

    result = calculate_isa_pressure(Metres(12000.0))
    assert_almost_equal(19330.38250807, result.v())

def test_calculate_isa_altitude():
    # calculate_troposphere_altitude
    result = calculate_isa_altitude(SEA_LEVEL_PRESSURE)
    assert 0.0 == result.v()

    result = calculate_isa_altitude(Pascals(105_000.0)) # 1050mB
    assert_almost_equal(-301.51854804303838, result.v())

    result = calculate_isa_altitude(Pascals(SEA_LEVEL_PRESSURE.v() - 10_000.0))
    assert_almost_equal(867.8115222838419, result.v())

    result = calculate_isa_altitude(Pascals(79495.20193405))
    assert_almost_equal(2000.0, result.v())

    result = calculate_isa_altitude(Pascals(60000.0)) # 600mB
    assert_almost_equal(4206.4224277251433, result.v())

    # calculate_tropopause_altitude
    result = calculate_isa_altitude(Pascals(22_632.040_095_007_81))
    assert_almost_equal(TROPOPAUSE_ALTITUDE.v(), result.v())

    result = calculate_isa_altitude(Pascals(19330.38250807))
    assert_almost_equal(12000.0, result.v())

def test_calculate_isa_temperature():
    result = calculate_isa_temperature(Metres(0.0), Kelvin(0.0))
    assert SEA_LEVEL_TEMPERATURE.v() == result.v()

    result = calculate_isa_temperature(Metres(500.0), Kelvin(0.0))
    assert SEA_LEVEL_TEMPERATURE.v() - 3.25 == result.v()

    result = calculate_isa_temperature(Metres(2000.0), Kelvin(0.0))
    assert SEA_LEVEL_TEMPERATURE.v() - 13.0 == result.v()

    result = calculate_isa_temperature(TROPOPAUSE_ALTITUDE, Kelvin(0.0))
    assert TROPOPAUSE_TEMPERATURE.v() == result.v()

def test_calculate_density():
    result = calculate_density(SEA_LEVEL_PRESSURE, SEA_LEVEL_TEMPERATURE)
    assert_almost_equal(SEA_LEVEL_DENSITY.v(), result.v())

    # Tropopause
    result = calculate_density(Pascals(22_632.040_095_007_81), TROPOPAUSE_TEMPERATURE)
    assert_almost_equal(0.36391765, result.v())

def test_calculate_true_air_speed():
    result = calculate_true_air_speed(MetresPerSecond(150.0), SEA_LEVEL_PRESSURE, SEA_LEVEL_TEMPERATURE)
    assert_almost_equal(150.0, result.v())

    # Calculate TAS at 2000 metres
    result = calculate_true_air_speed(MetresPerSecond(150.0),
        Pascals(79495.202), Kelvin(SEA_LEVEL_TEMPERATURE.v() - 13.0))
    assert_almost_equal(164.457894, result.v())

def test_calculate_calibrated_air_speed():
    result = calculate_calibrated_air_speed(MetresPerSecond(150.0), SEA_LEVEL_PRESSURE, SEA_LEVEL_TEMPERATURE)
    assert_almost_equal(150.0, result.v())

    # Calculate CAS at 2000 metres
    result = calculate_calibrated_air_speed(MetresPerSecond(164.457894),
        Pascals(79495.202), Kelvin(SEA_LEVEL_TEMPERATURE.v() - 13.0))
    assert_almost_equal(150.0, result.v())

def test_speed_of_sound():
    result = speed_of_sound(SEA_LEVEL_TEMPERATURE)
    assert_almost_equal(SEA_LEVEL_SPEED_OF_SOUND.v(), result.v(), 5)

    result = speed_of_sound(TROPOPAUSE_TEMPERATURE)
    assert_almost_equal(295.0694935, result.v())

def test_mach_true_air_speed():
    result = mach_true_air_speed(0.8, SEA_LEVEL_TEMPERATURE)
    assert_almost_equal(0.8 * SEA_LEVEL_SPEED_OF_SOUND.v(), result.v(), 5)

    result = mach_true_air_speed(0.85, TROPOPAUSE_TEMPERATURE)
    assert_almost_equal(0.85 * 295.0694935, result.v())

def test_calculate_crossover_altitude():
    cas = MetresPerSecond(155.0)
    mach = 0.79
    crossover_altitude = calculate_crossover_altitude(cas, mach)
    assert_almost_equal(9070.813566, crossover_altitude.v())

    # The TAS should be the same from both CAS and MACH at the crossover_altitude
    pressure = calculate_isa_pressure(crossover_altitude)
    temperature = calculate_isa_temperature(crossover_altitude, Kelvin(0.0))
    tas_from_cas = calculate_true_air_speed(cas, pressure, temperature)
    tas_from_mach = mach_true_air_speed(0.79, temperature)
    assert_almost_equal(tas_from_cas.v(), tas_from_mach.v(), 5)
    assert_almost_equal(239.75607215, tas_from_mach.v())

if __name__ == '__main__':
    pytest.main()
