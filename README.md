# via-isa-cpp

[![License](https://img.shields.io/badge/License-MIT-blue)](https://opensource.org/license/mit/)
[![C/C++ CI](https://github.com/kenba/via-isa-cpp/workflows/C/C++%20CI/badge.svg)](https://github.com/kenba/via-isa-cpp/actions)
[![codecov](https://codecov.io/gh/kenba/via-isa-cpp/graph/badge.svg?token=EPTTOGTZTI)](https://codecov.io/gh/kenba/via-isa-cpp)

An implementation of the [International Civil Aviation Organization](https://icao.int/) (ICAO) [International Standard Atmosphere](https://en.wikipedia.org/wiki/International_Standard_Atmosphere) (ISA), see [ICAO Doc 7488/3](https://standart.aero/en/icao/book/doc-7488-manual-of-the-icao-standard-atmosphere-extended-to-80-kilometres-262-500-feet-en-cons).

The library also includes functions for calculating:

- true airspeed ([TAS](https://en.wikipedia.org/wiki/True_airspeed)) from calibrated airspeed ([CAS](https://en.wikipedia.org/wiki/Calibrated_airspeed)), pressure and temperature;
- CAS from TAS, pressure and temperature;
- TAS from [Mach number](https://en.wikipedia.org/wiki/Mach_number) and temperature;
- and the crossover altitude between CAS / Mach flight regimes.

The equations for the functions above are from [BADA User Manual revision 3-12](https://www.scribd.com/document/289480324/1-User-Manual-Bada-3-12).

## Example

The following example calculates the crossover altitude for an aircraft given
its CAS in `MetresPerSecond` and its `Mach` number:  

### C++
```C++
#include "via/isa.hpp"
#include <boost/test/unit_test.hpp>

using namespace via::isa;
using namespace via::units::si;

namespace {
const double CALCULATION_TOLERANCE(1.0e-6);
} // namespace

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
```

### Python
```Python
import pytest
from numpy.testing import assert_almost_equal
from via_units import Kelvin, Metres, MetresPerSecond, Pascals
from via_isa import calculate_crossover_altitude, calculate_isa_pressure,\
calculate_isa_temperature, calculate_true_air_speed, mach_true_air_speed

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
```

## Use

The C++ software depends on:

* the [via-units-cpp](https://github.com/kenba/via-units-cpp) library;
* and the Microsoft [GSL](https://github.com/microsoft/GSL) library to provide [Contracts](https://isocpp.org/files/papers/P2900R6.pdf) support.

The C++ tests use the [boost.test](https://www.boost.org/doc/libs/1_86_0/libs/test/doc/html/boost_test/intro.html)
library.

### C++

#### Installation

The library is header only, so the library `include` directory just needs to be added to the include path.
Alternatively, when using [cmake](https://cmake.org/) the environment variable `ViaIsa_DIR` just needs
to be set to the location of the `via-isa-cpp` directory; `cmake` will add it to the include path.

Note: `CMakeLists.txt` is setup to install python by default, so `-DINSTALL_PYTHON=OFF`
must be passed to `cmake` when building for C++.

`cmake` can also be used to install the library to the relevant include directory on Linux/macOS.
In the `via-isa-cpp` directory, run:

```bash
cmake -DINSTALL_PYTHON=OFF .
sudo make install
```

Note: it will be necessary to delete the `CMakeCache.txt` file created by
running `cmake` above, before running `cmake` on this library again.

#### Tests

The C++ tests can be built and run using `cmake` by running:

```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DINSTALL_PYTHON=OFF -DCPP_UNIT_TESTS=ON <via-isa-cpp directory>
make
make test
```

Note: `-DCMAKE_EXPORT_COMPILE_COMMANDS=1` creates a `compile_commands.json`
file which can be copied back into the `via-isa-cpp` directory for
[clangd](https://clangd.llvm.org/) tools.

### Python

The library uses [pybind11](https://github.com/pybind/pybind11) to provide C++ Python bindings
and [scikit-build](https://pypi.org/project/scikit-build/) to build a python package using
[cmake](https://cmake.org/).

From the parent directory of `via-isa-cpp`, run:

```bash
pip install ./via-isa-cpp
```

In Python code import the software as `via_isa`, e.g.:

```python
from via_units import Kelvin, Metres, MetresPerSecond, Pascals
from via_isa import calculate_isa_pressure, calculate_isa_temperature, calculate_true_air_speed,\
mach_true_air_speed, calculate_crossover_altitude,
```

Note: `via_units` must also be imported for the units used by the `via_isa` functions.

See: [test_isa.py](python/tests/test_isa.py).

## License

`icao-isa-rs` is provided under a MIT license, see [LICENSE](LICENSE).
