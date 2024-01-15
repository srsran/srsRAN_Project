/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/support/backtrace.h"
#include <string>

/// \file
/// \brief Test binary to test backtrace printing.
///
/// The app provokes a segmentation fault. If backtrace printing using backward-cpp has been compiled in you
/// should be getting a nice call stack trace when launching the app.

void function2(bool do_segfault)
{
  if (do_segfault) {
    int* foo = nullptr;
    *foo     = 1;
  }
}

int function1(std::string param)
{
  function2(param == "segfault");
  return 0;
}

int main(int argc, char** argv)
{
  // Enable backtrace.
  srsran::enable_backtrace();

  return function1("segfault");
}