/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
      // Suppress a race in fmt::print that is guarded by flockfile.
      "race:fmt::*\n"
      // External uninstrumented libraries.
      "called_from_lib:libzmq.so\n"
      "called_from_lib:libpgm-5.2.so\n"
      "called_from_lib:libusb*\n"
      "called_from_lib:libuhd*\n"
      "called_from_lib:librte_eal*\n"
      "called_from_lib:librte_net_iavf*\n"
      // Races detected inside uninstrumented libraries. This may hide legit races if any of the libraries appear in the
      // backtrace.
      "race:libusb*\n"
      "race:libuhd*\n";
}

#ifdef __cplusplus
}
#endif
