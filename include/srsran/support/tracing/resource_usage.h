/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/support/format/fmt_basic_parser.h"
#include <chrono>

namespace srsran {
namespace resource_usage {

/// Difference between two resource_usage::snapshot points for a given thread.
struct diff {
  /// Number of voluntary context switches between the two snapshots.
  long vol_ctxt_switch_count;
  /// Number of involuntary context switches between the two snapshots.
  long invol_ctxt_switch_count;
  /// Time spent in user mode between two snapshots.
  std::chrono::microseconds user_time{0};
  /// Time spent in kernel mode between two snapshots.
  std::chrono::microseconds sys_time{0};
};

/// Snapshot of the resource usage statistics of a specific thread at given point in time.
struct snapshot {
  /// Number of voluntary context switches at this point in time.
  long int vol_ctxt_switch_count = 0;
  /// Number of involuntary context switches at this point in time.
  long int invol_ctxt_switch_count = 0;
  /// Time spent in user mode.
  std::chrono::microseconds user_time{0};
  /// Time spent in kernel mode.
  std::chrono::microseconds sys_time{0};

  constexpr snapshot() {}
  constexpr snapshot(long int                  vol_cswitch_,
                     long int                  invol_switch_,
                     std::chrono::microseconds user_time_,
                     std::chrono::microseconds sys_time_) :
    vol_ctxt_switch_count(vol_cswitch_),
    invol_ctxt_switch_count(invol_switch_),
    user_time(user_time_),
    sys_time(sys_time_)
  {
  }

  /// \brief Computes the difference between two resource_usage::snapshot points.
  /// \remark Both snapshots should be captured in the same thread.
  resource_usage::diff operator-(const resource_usage::snapshot& rhs) const;
};

/// Collect a new snapshot for the caller thread.
srsran::expected<snapshot, int> now();

} // namespace resource_usage
} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::resource_usage::diff> : public basic_parser {
  template <typename FormatContext>
  auto format(const srsran::resource_usage::diff& diff, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "nof_vol_ctxt_switch={} nof_invol_ctxt_switch={}",
                     diff.vol_ctxt_switch_count,
                     diff.invol_ctxt_switch_count);
    return ctx.out();
  }
};

} // namespace fmt
