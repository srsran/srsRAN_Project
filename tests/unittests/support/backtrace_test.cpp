/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/backtrace.h"
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
  srsgnb::enable_backtrace();

  return function1("segfault");
}