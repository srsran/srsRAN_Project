/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/tracing/resource_usage.h"
#include <sys/time.h>

using namespace srsran;
using namespace resource_usage;

srsran::expected<snapshot, int> srsran::resource_usage::now()
{
  snapshot ret;
  if (getrusage(RUSAGE_SELF, &ret) == 0) {
    return ret;
  }
  return make_unexpected(errno);
}

diff resource_usage::snapshot::operator-(const snapshot& rhs) const
{
  diff           ret;
  struct timeval diff_time;
  timersub(&ru_utime, &rhs.ru_utime, &diff_time);
  ret.user_dur = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds{diff_time.tv_sec}) +
                 std::chrono::microseconds{diff_time.tv_usec};
  timersub(&ru_stime, &rhs.ru_stime, &diff_time);
  ret.system_dur              = std::chrono::seconds{diff_time.tv_sec} + std::chrono::microseconds{diff_time.tv_usec};
  ret.vol_ctxt_switch_count   = ru_nvcsw - rhs.ru_nvcsw;
  ret.invol_ctxt_switch_count = ru_nivcsw - rhs.ru_nivcsw;
  return ret;
}
