/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/signal_handler.h"
#include "fmt/format.h"
#include <atomic>
#include <csignal>
#include <cstdio>
#include <unistd.h>

#ifndef TERM_TIMEOUT_S
#define TERM_TIMEOUT_S (5)
#endif

/// Handler called after the user interrupts the program.
static std::atomic<srsran_signal_handler> user_handler;

static void signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      fmt::print(stderr, "Couldn't stop after {}s. Forcing exit.\n", TERM_TIMEOUT_S);
      ::raise(SIGKILL);
    default:
      // all other registered signals try to stop the app gracefully
      // Call the user handler if present and remove it so that further signals are treated by the default handler.
      if (auto handler = user_handler.exchange(nullptr)) {
        handler();
      } else {
        return;
      }
      alarm(TERM_TIMEOUT_S);
      break;
  }
}

void register_signal_handler(srsran_signal_handler handler)
{
  user_handler.store(handler);

  ::signal(SIGINT, signal_handler);
  ::signal(SIGTERM, signal_handler);
  ::signal(SIGHUP, signal_handler);
  ::signal(SIGALRM, signal_handler);
}
