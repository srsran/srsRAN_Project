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
#include <sys/resource.h>

using namespace srsran;
using namespace resource_usage;

static snapshot rusage_to_snapshot(const ::rusage& rusg)
{
  snapshot s;
  s.user_time   = std::chrono::seconds{rusg.ru_utime.tv_sec} + std::chrono::microseconds{rusg.ru_utime.tv_usec};
  s.system_time = std::chrono::seconds{rusg.ru_stime.tv_sec} + std::chrono::microseconds{rusg.ru_stime.tv_usec};
  s.vol_ctxt_switch_count   = rusg.ru_nvcsw;
  s.invol_ctxt_switch_count = rusg.ru_nivcsw;
  return s;
}

srsran::expected<snapshot, int> srsran::resource_usage::now()
{
  ::rusage ret;
  if (getrusage(RUSAGE_THREAD, &ret) == 0) {
    return rusage_to_snapshot(ret);
  }
  return make_unexpected(errno);
}

diff resource_usage::snapshot::operator-(const snapshot& rhs) const
{
  diff ret;
  ret.user_dur                = user_time - rhs.user_time;
  ret.system_dur              = system_time - rhs.system_time;
  ret.vol_ctxt_switch_count   = vol_ctxt_switch_count - rhs.vol_ctxt_switch_count;
  ret.invol_ctxt_switch_count = invol_ctxt_switch_count - rhs.invol_ctxt_switch_count;
  return ret;
}
