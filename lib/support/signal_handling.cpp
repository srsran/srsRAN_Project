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

#include "srsran/support/signal_handling.h"
#include "fmt/core.h"
#include <atomic>
#include <csignal>
#include <cstdio>
#include <unistd.h>

using namespace srsran;

/// Termination timeout value measured in seconds.
static constexpr unsigned TERMINATION_TIMEOUT_S =
#ifndef TERM_TIMEOUT_S
    5;
#else
    TERM_TIMEOUT_S;
#endif

/// Handler called after the user interrupts the program.
static std::atomic<srsran_signal_handler> interrupt_handler = nullptr;
/// Handler called just before forcing application exit.
static std::atomic<srsran_signal_handler> cleanup_handler = nullptr;

static void signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      fmt::print(stderr, "Could not stop application after {} seconds. Forcing exit.\n", TERMINATION_TIMEOUT_S);
      if (auto handler = cleanup_handler.exchange(nullptr)) {
        handler();
      }
      ::raise(SIGKILL);
      [[fallthrough]];
    default:
      // All other registered signals try to stop the application gracefully.
      // Call the user handler, if present, and remove it so that further signals are treated by the default handler.
      if (auto handler = interrupt_handler.exchange(nullptr)) {
        handler();
      } else {
        return;
      }
      ::alarm(TERMINATION_TIMEOUT_S);
      break;
  }
}

void srsran::register_interrupt_signal_handler(srsran_signal_handler handler)
{
  interrupt_handler.store(handler);

  ::signal(SIGINT, signal_handler);
  ::signal(SIGTERM, signal_handler);
  ::signal(SIGHUP, signal_handler);
  ::signal(SIGALRM, signal_handler);
}

void srsran::register_cleanup_signal_handler(srsran_signal_handler handler)
{
  cleanup_handler.store(handler);
}
