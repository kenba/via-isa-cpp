#pragma once

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
//////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The via-isa-cpp library header file.
//////////////////////////////////////////////////////////////////////////////
/// @mainpage via-isa-cpp
///
/// The library defines:
#include "isa/constants.hpp"
#include <cmath>
#include <gsl/assert>

namespace via {
namespace isa {

/// The coefficient used in CAS / TAS conversions.
/// See BADA Equation 3.2-14
template <typename T>
  requires std::floating_point<T>
constexpr T U{(constants::K<T> - T(1)) / constants::K<T>};

// Another coefficient used in pressure conversions.
// See BADA Equation 3.2-14
template <typename T>
  requires std::floating_point<T>
constexpr T INV_U{T(1) / U<T>};

/// The Power factor used in calculating the pressure below the Tropopause.
/// See BADA Equation 3.1-18
template <typename T>
  requires std::floating_point<T>
constexpr T PRESSURE_POWER{
    -constants::g<T>.v() /
    (constants::TEMPERATURE_GRADIENT<T> * constants::R<T>)};

/// The Power factor used in calculating the altitude below the Tropopause.
/// See BADA Equation 3.1-8
template <typename T>
  requires std::floating_point<T>
constexpr T TEMPERATURE_POWER{T(1) / PRESSURE_POWER<T>};

/// The pressure at `TROPOPAUSE_ALTITUDE` in Pascals.
/// See BADA Equation Eq 3.1-19
template <typename T>
  requires std::floating_point<T>
constexpr units::si::Pascals<T> TROPOPAUSE_PRESSURE{22'632.040'095'007'81};

/// The factor used in calculating the density and pressure above Tropopause.
/// See BADA Equation 3.2-16
template <typename T>
  requires std::floating_point<T>
constexpr T TROPOPAUSE_PRESSURE_FACTOR{
    -constants::g<T>.v() /
    (constants::R<T> * constants::TROPOPAUSE_TEMPERATURE<T>.v())};

/// Calculate the ISA pressure below the tropopause for the given altitude.
/// See BADA Rev 3.12, Eq 3.1-18
/// @pre altitude <= TROPOPAUSE_ALTITUDE
/// @param the pressure altitude in metres.
/// @return the pressure in Pascals.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto
calculate_troposphere_pressure(const units::si::Metres<T> altitude)
    -> units::si::Pascals<T> {
  Expects(altitude <= constants::TROPOPAUSE_ALTITUDE<T>);

  return units::si::Pascals<T>(
      constants::SEA_LEVEL_PRESSURE<T>.v() *
      std::pow(T(1) + altitude.v() * constants::TEMPERATURE_GRADIENT<T> /
                          constants::SEA_LEVEL_TEMPERATURE<T>.v(),
               PRESSURE_POWER<T>));
}

/// Calculate the ISA pressure in the tropopause for the given altitude.
/// See BADA Rev 3.12, Eq 3.1-20
/// @pre altitude >= TROPOPAUSE_ALTITUDE
/// @param the pressure altitude in metres.
/// @return the pressure in Pascals.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto
calculate_tropopause_pressure(const units::si::Metres<T> altitude)
    -> units::si::Pascals<T> {
  Expects(altitude >= constants::TROPOPAUSE_ALTITUDE<T>);

  return units::si::Pascals<T>(
      TROPOPAUSE_PRESSURE<T>.v() *
      std::exp(TROPOPAUSE_PRESSURE_FACTOR<T> *
               (altitude.v() - constants::TROPOPAUSE_ALTITUDE<T>.v())));
}

/// Calculate the ISA pressure corresponding to the given altitude.
/// See BADA Rev 3.12, Eq 3.1-18 & Eq 3.1-20
/// @param altitude the pressure altitude in metres.
/// @return the pressure in Pascals.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto calculate_isa_pressure(const units::si::Metres<T> altitude)
    -> units::si::Pascals<T> {
  return (altitude < constants::TROPOPAUSE_ALTITUDE<T>)
             ? calculate_troposphere_pressure(altitude)
             : calculate_tropopause_pressure(altitude);
}

/// Calculate the altitude corresponding to the given pressure below the
/// tropopause.
/// See BADA Rev 3.12, Eq 3.1-8
/// @param pressure the pressure in Pascals.
/// @return the altitude in metres.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto
calculate_troposphere_altitude(const units::si::Pascals<T> pressure)
    -> units::si::Metres<T> {
  const auto pressure_ratio{pressure.v() /
                            constants::SEA_LEVEL_PRESSURE<T>.v()};
  const auto altitude_ratio{std::pow(pressure_ratio, TEMPERATURE_POWER<T>) -
                            T(1)};
  return units::si::Metres<T>(altitude_ratio *
                              constants::SEA_LEVEL_TEMPERATURE<T>.v() /
                              constants::TEMPERATURE_GRADIENT<T>);
}

/// Calculate the altitude corresponding to the given pressure in the
/// tropopause.
/// See BADA Rev 3.12, Eq 3.1-20
/// @param pressure the pressure in Pascals.
/// @return the altitude in metres.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto
calculate_tropopause_altitude(const units::si::Pascals<T> pressure)
    -> units::si::Metres<T> {
  const auto altitude_delta{
      std::log(pressure.v() / TROPOPAUSE_PRESSURE<T>.v()) /
      TROPOPAUSE_PRESSURE_FACTOR<T>};
  return units::si::Metres<T>(constants::TROPOPAUSE_ALTITUDE<T>.v() +
                              altitude_delta);
}

/// Calculate the altitude corresponding to the given pressure.
/// See BADA Rev 3.12, Eq 3.1-18 & Eq 3.1-20
/// @param pressure the pressure in Pascals.
/// @return the altitude in metres.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto calculate_isa_altitude(const units::si::Pascals<T> pressure) {
  return (pressure > TROPOPAUSE_PRESSURE<T>)
             ? calculate_troposphere_altitude(pressure)
             : calculate_tropopause_altitude(pressure);
}

/// Calculate the ISA temperature corresponding to the given altitude and
/// difference in Sea level temperature.
/// See ICAO Doc 7488/3, Eq (11)
/// @param altitude the pressure altitude in Metres.
/// @param delta_temperature the difference from ISA temperature at Sea level,
/// default zero.
/// @return the temperature in Kelvin.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto calculate_isa_temperature(
    const units::si::Metres<T> altitude,
    units::si::Kelvin<T> delta_temperature = units::si::Kelvin<T>(0))
    -> units::si::Kelvin<T> {
  const auto temperature{units::si::Kelvin<T>(
      constants::SEA_LEVEL_TEMPERATURE<T>.v() + delta_temperature.v() +
      constants::TEMPERATURE_GRADIENT<T> * altitude.v())};

  return (temperature > constants::TROPOPAUSE_TEMPERATURE<T>)
             ? temperature
             : constants::TROPOPAUSE_TEMPERATURE<T>;
}

/// Calculate the air density given the air temperature and pressure.
/// Use Ideal Gas Equation (Boyles law)
/// See See ICAO Doc 7488/3, Eq (3).
/// @pre temperature > 0
/// @param pressure the pressure in Pascals.
/// @param temperature the temperature in Kelvin.
/// @return the density in Kg per cubic metre.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto calculate_density(const units::si::Pascals<T> pressure,
                                 const units::si::Kelvin<T> temperature)
    -> units::si::KilogramsPerCubicMetre<T> {
  Expects(temperature.v() > T());

  return units::si::KilogramsPerCubicMetre<T>(
      pressure.v() / (constants::R<T> * temperature.v()));
}

/// Calculate the True Air Speed (TAS) from the Calibrated Air Speed (CAS)
/// at the given pressure and temperature.
/// See BADA Rev 3.12, Eq 3.1.23
/// @param cas the Calibrated Air Speed in metres per second.
/// @param pressure the pressure in Pascals.
/// @param temperature the temperature in Kelvin.
/// @return the True Air Speed in metres per second.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto calculate_true_air_speed(const units::si::MetresPerSecond<T> cas,
                                        const units::si::Pascals<T> pressure,
                                        const units::si::Kelvin<T> temperature)
    -> units::si::MetresPerSecond<T> {
  constexpr T INNER_FACTOR{U<T> / (T(2) * constants::R<T> *
                                   constants::SEA_LEVEL_TEMPERATURE<T>.v())};
  constexpr T OUTER_FACTOR{T(2) * constants::R<T> / U<T>};

  const T cas_factor{
      std::pow(T(1) + INNER_FACTOR * cas.v() * cas.v(), INV_U<T>) - T(1)};
  const T cas_pressure_factor{
      std::pow(T(1) + constants::SEA_LEVEL_PRESSURE<T>.v() * cas_factor /
                          pressure.v(),
               U<T>) -
      T(1)};

  return units::si::MetresPerSecond<T>(
      std::sqrt(OUTER_FACTOR * temperature.v() * cas_pressure_factor));
}

/// Calculate the Calibrated Air Speed (CAS) from the True Air Speed (TAS)
/// at the given pressure and temperature.
/// See BADA Rev 3.12, Eq 3.1.24
/// @param tas the True Air Speed in metres per second.
/// @param pressure the pressure in Pascals.
/// @param temperature the temperature in Kelvin.
/// @return the Calibrated Air Speed in metres per second.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto
calculate_calibrated_air_speed(const units::si::MetresPerSecond<T> tas,
                               const units::si::Pascals<T> pressure,
                               const units::si::Kelvin<T> temperature)
    -> units::si::MetresPerSecond<T> {
  constexpr T INNER_FACTOR{U<T> / (T(2) * constants::R<T>)};
  constexpr T OUTER_FACTOR{T(2) * constants::R<T> *
                           constants::SEA_LEVEL_TEMPERATURE<T>.v() / U<T>};

  const T tas_factor =
      std::pow(T(1) + INNER_FACTOR * tas.v() * tas.v() / temperature.v(),
               INV_U<T>) -
      T(1);
  const T tas_pressure_factor =
      std::pow(T(1) + pressure.v() * tas_factor /
                          constants::SEA_LEVEL_PRESSURE<T>.v(),
               U<T>) -
      T(1);

  return units::si::MetresPerSecond<T>(
      std::sqrt(OUTER_FACTOR * tas_pressure_factor));
}

/// Calculate the speed of sound for the given temperature.
/// See ICAO Doc 7488/3, Eq (21).
/// @pre 0.0 < temperature
/// @param temperature the temperature in Kelvin.
/// @return the speed of sound in metres per second.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto speed_of_sound(const units::si::Kelvin<T> temperature)
    -> units::si::MetresPerSecond<T> {
  Expects(temperature.v() > T());

  return units::si::MetresPerSecond<T>(
      std::sqrt(constants::K<T> * constants::R<T> * temperature.v()));
}

/// Calculate the True Air Speed (TAS) from the Mach number at the
/// given temperature.
/// See BADA Rev 3.12, Eq 3.1.22
/// @pre mach > 0.0
/// @param mach the Mach number.
/// @param temperature the temperature in Kelvin.
/// @return the True Air Speed in metres per second.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto mach_true_air_speed(const T mach,
                                   const units::si::Kelvin<T> temperature)
    -> units::si::MetresPerSecond<T> {
  Expects(mach > T());

  return units::si::MetresPerSecond<T>(mach *
                                       speed_of_sound<T>(temperature).v());
}

/// This function calculates the crossover pressure ratio between the
/// Calibrated Air Speed and Mach number.
/// See BADA Rev 3.12, Eq 3.1-29
/// @pre mach > 0.0
/// @param cas the Calibrated Air Speed in metres per second.
/// @param mach the Mach number.
/// @return the pressure ratio.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto
calculate_crossover_pressure_ratio(const units::si::MetresPerSecond<T> cas,
                                   const T mach) -> T {
  constexpr T K_MINUS_1_OVER_2{(constants::K<T> - T(1)) / T(2)};

  Expects(mach > T());

  const T cas_mach{cas.v() / constants::SEA_LEVEL_SPEED_OF_SOUND<T>.v()};
  const T numerator{
      std::pow(T(1) + K_MINUS_1_OVER_2 * cas_mach * cas_mach, INV_U<T>) - T(1)};
  const T denominator{
      std::pow(T(1) + K_MINUS_1_OVER_2 * mach * mach, INV_U<T>) - T(1)};
  return numerator / denominator;
}

/// Calculate the crossover altitude at which the True Air Speeds (TAS)
/// corresponding to the given Calibrated Air Speed (CAS) and Mach number are
/// the same.
/// See BADA Rev 3.12, Eq 3.1-27
/// @pre mach > 0.0
/// @param cas the Calibrated Air Speed in metres per second.
/// @param mach the Mach number.
/// @return the altitude in metres.
template <typename T>
  requires std::floating_point<T>
[[nodiscard("Pure Function")]]
constexpr auto
calculate_crossover_altitude(const units::si::MetresPerSecond<T> cas,
                             const T mach) -> units::si::Metres<T> {
  Expects(mach > T());

  const T temperature_ratio{std::pow(
      calculate_crossover_pressure_ratio(cas, mach), TEMPERATURE_POWER<T>)};
  return units::si::Metres<T>(constants::SEA_LEVEL_TEMPERATURE<T>.v() *
                              (T(1) - temperature_ratio) /
                              -constants::TEMPERATURE_GRADIENT<T>);
}

} // namespace isa
} // namespace via
