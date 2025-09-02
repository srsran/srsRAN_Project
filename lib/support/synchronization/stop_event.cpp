/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/synchronization/stop_event.h"
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

using namespace srsran;

void stop_event_token::wake_all()
{
  // Note: Futex requires int*.
  // Note: No C++ aliasing is happening, as the kernel will just copy the uint32 bytes.
  auto* addr = reinterpret_cast<int*>(count_ptr);
  ::syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, INT32_MAX, nullptr, nullptr, 0);
}

void stop_event_source::wait(uint32_t expected)
{
  // Note: The kernel will only sleep if *addr == expected; otherwise returns -1/EAGAIN.
  // Note: Futex requires int*.
  // Note: No C++ aliasing is happening, as the kernel will just copy the uint32 bytes.
  // Note: Spurious wakeups or EINTR are fine; callers re-check the condition in a loop.
  auto* addr = reinterpret_cast<int*>(&state);
  ::syscall(SYS_futex, addr, FUTEX_WAIT_PRIVATE, expected, nullptr, nullptr, 0);
}
