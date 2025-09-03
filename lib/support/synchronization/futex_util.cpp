/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/synchronization/futex_util.h"
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

using namespace srsran;

long futex_util::wait(std::atomic<uint32_t>& state, uint32_t expected)
{
  // The kernel will only sleep if *addr == expected; otherwise returns -1/EAGAIN.
  // Note: Futex requires int*.
  // Note: No C++ aliasing is happening, as the kernel will just copy the uint32 bytes.
  auto* addr = reinterpret_cast<int*>(&state);
  return ::syscall(SYS_futex, addr, FUTEX_WAIT_PRIVATE, expected, nullptr, nullptr, 0);
}

long futex_util::wake_all(std::atomic<uint32_t>& state)
{
  // Note: Futex requires int*.
  // Note: No C++ aliasing is happening, as the kernel will just copy the uint32 bytes.
  auto* addr = reinterpret_cast<int*>(&state);
  return ::syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, INT32_MAX, nullptr, nullptr, 0);
}
