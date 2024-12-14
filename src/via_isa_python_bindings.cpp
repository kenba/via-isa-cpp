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
/// @file via_isa_python_bindings.cpp
/// @brief Contains the via::isa python interface
//////////////////////////////////////////////////////////////////////////////
#include "via/isa.hpp"
#include <pybind11/pybind11.h>

PYBIND11_MODULE(via_isa, m) {
  // Python bindings for ISA constants
  m.attr("g") = via::isa::constants::g<double>;
  m.attr("K") = via::isa::constants::K<double>;
  m.attr("R") = via::isa::constants::R<double>;
  m.attr("SEA_LEVEL_TEMPERATURE") =
      via::isa::constants::SEA_LEVEL_TEMPERATURE<double>;
  m.attr("SEA_LEVEL_PRESSURE") =
      via::isa::constants::SEA_LEVEL_PRESSURE<double>;
  m.attr("SEA_LEVEL_DENSITY") = via::isa::constants::SEA_LEVEL_DENSITY<double>;
  m.attr("SEA_LEVEL_SPEED_OF_SOUND") =
      via::isa::constants::SEA_LEVEL_SPEED_OF_SOUND<double>;
  m.attr("TROPOPAUSE_TEMPERATURE") =
      via::isa::constants::TROPOPAUSE_TEMPERATURE<double>;
  m.attr("TEMPERATURE_GRADIENT") =
      via::isa::constants::TEMPERATURE_GRADIENT<double>;
  m.attr("TROPOPAUSE_ALTITUDE") =
      via::isa::constants::TROPOPAUSE_ALTITUDE<double>;

  // Python bindings for ISA functions
  m.def("calculate_isa_pressure", &via::isa::calculate_isa_pressure<double>,
        "Calculate the ISA pressure corresponding to the given altitude.");
  m.def("calculate_isa_altitude", &via::isa::calculate_isa_altitude<double>,
        "Calculate the ISA altitude corresponding to the given pressure.");
  m.def("calculate_isa_temperature",
        &via::isa::calculate_isa_temperature<double>,
        "Calculate the ISA temperature corresponding to the given altitude and "
        "difference in Sea level temperature.");
  m.def("calculate_density", &via::isa::calculate_density<double>,
        "Calculate the air density given the air temperature and pressure.");
  m.def("calculate_true_air_speed", &via::isa::calculate_true_air_speed<double>,
        "Calculate the True Air Speed (TAS) from the Calibrated Air Speed "
        "(CAS) at the given pressure and temperature.");
  m.def("calculate_calibrated_air_speed",
        &via::isa::calculate_calibrated_air_speed<double>,
        "Calculate the Calibrated Air Speed (CAS) from the True Air Speed "
        "(TAS) at the given pressure and temperature.");
  m.def("speed_of_sound", &via::isa::speed_of_sound<double>,
        "Calculate the speed of sound for the given temperature.");
  m.def("mach_true_air_speed", &via::isa::mach_true_air_speed<double>,
        "Calculate the True Air Speed (TAS) from the Mach number at the given "
        "temperature.");
  m.def("calculate_crossover_altitude",
        &via::isa::calculate_crossover_altitude<double>,
        "Calculate the crossover altitude at which the True Air Speeds (TAS) "
        "corresponding to the given Calibrated Air Speed (CAS) and Mach number "
        "are the same.");
}
