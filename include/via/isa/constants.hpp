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
/// @brief The ICAO Standard Atmosphere primary constants and characteristics.
/// @see Manual of the ICAO Standard Atmosphere; ICAO Doc 7488/3.
//////////////////////////////////////////////////////////////////////////////
#include <via/units.hpp>

namespace via {
namespace isa {
namespace constants {
// Constants from ICAO Doc 7488/3, Table A.

/// The acceleration due to gravity at latitude 45°32'33'' using Lambert's
/// equation.
template <typename T>
  requires std::floating_point<T>
constexpr units::si::MetresPerSecondSquared<T> g{9.806'65};

/// The adiabatic index of air (K), dimensionless.
template <typename T>
  requires std::floating_point<T>
constexpr T K{1.4};

/// The real gas constant for air (R) in metres squared per Kelvin seconds
/// squared.
template <typename T>
  requires std::floating_point<T>
constexpr T R{287.052'87};

/// ISA Sea level temperature.
template <typename T>
  requires std::floating_point<T>
constexpr units::si::Kelvin<T> SEA_LEVEL_TEMPERATURE{288.15};

/// ISA Sea level pressure.
template <typename T>
  requires std::floating_point<T>
constexpr units::si::Pascals<T> SEA_LEVEL_PRESSURE{101'325.0};

/// ISA Sea level density.
template <typename T>
  requires std::floating_point<T>
constexpr units::si::KilogramsPerCubicMetre<T> SEA_LEVEL_DENSITY{1.225};

/// ISA Sea level speed of sound (a0).
/// ICAO Doc 7488/3, Table C.
template <typename T>
  requires std::floating_point<T>
constexpr units::si::MetresPerSecond<T> SEA_LEVEL_SPEED_OF_SOUND{340.294};

// Constants from ICAO Doc 7488/3, Table D.

/// ISA tropopause temperature.
template <typename T>
  requires std::floating_point<T>
constexpr units::si::Kelvin<T> TROPOPAUSE_TEMPERATURE{216.65};

/// The ISA temperature gradient from Sea level to the tropopause altitude in
/// K/m. AKA Lapse Rate.
template <typename T>
  requires std::floating_point<T>
constexpr T TEMPERATURE_GRADIENT{-0.006'5};

/// ISA tropopause altitude.
template <typename T>
  requires std::floating_point<T>
constexpr units::si::Metres<T> TROPOPAUSE_ALTITUDE{11'000.0};

} // namespace constants
} // namespace isa
} // namespace via
