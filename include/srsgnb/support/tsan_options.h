/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// Options taken from Mozilla project
//   abort_on_error=1 - Causes TSan to abort instead of using exit().
//   halt_on_error=1 - Causes TSan to stop on the first race detected.
//
//   report_signal_unsafe=0 - Required to avoid TSan deadlocks when
//   receiving external signals (e.g. SIGINT manually on console).
//
//   allocator_may_return_null=1 - Tell TSan to return NULL when an allocation
//   fails instead of aborting the program. This allows us to handle failing
//   allocations the same way we would handle them with a regular allocator and
//   also uncovers potential bugs that might occur in these situations.

#ifdef __cplusplus
extern "C" {
#endif

const char* __tsan_default_options()
{
  return "halt_on_error=1:abort_on_error=1:report_signal_unsafe=0"
         ":allocator_may_return_null=1";
}

const char* __tsan_default_suppressions()
{
  return
      // External uninstrumented libraries
      "called_from_lib:libzmq.so\n"
      "called_from_lib:libpgm-5.2.so\n"
      "called_from_lib:libusb*\n"
      "called_from_lib:libuhd*\n"
      // Races detected inside uninstrumented libraries. This may hide legit races if any of the libraries appear in the
      // backtrace
      "race:libusb*\n"
      "race:libuhd*\n";
}

#ifdef __cplusplus
}
#endif