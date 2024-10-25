/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  /// User CPU time elapsed between the two snapshots.
  std::chrono::microseconds user_dur;
  /// System CPU time elapsed between the two snapshots.
  std::chrono::microseconds system_dur;
  /// Number of voluntary context switches between the two snapshots.
  long vol_ctxt_switch_count;
  /// Number of involuntary context switches between the two snapshots.
  long invol_ctxt_switch_count;
};

/// Snapshot of the resource usage statistics of a specific thread at given point in time.
struct snapshot {
  /// User CPU time.
  std::chrono::microseconds user_time;
  /// System CPU time.
  std::chrono::microseconds system_time;
  /// Number of voluntary context switches at this point in time.
  long int vol_ctxt_switch_count;
  /// Number of involuntary context switches at this point in time.
  long int invol_ctxt_switch_count;

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
  auto format(const srsran::resource_usage::diff& diff, FormatContext& ctx)
  {
    return format_to(ctx.out(),
                     "user_time={}usec system_time={}usec nof_vol_ctxt_switch={} nof_invol_ctxt_switch={}",
                     diff.user_dur.count(),
                     diff.system_dur.count(),
                     diff.vol_ctxt_switch_count,
                     diff.invol_ctxt_switch_count);
    return ctx.out();
  }
};

} // namespace fmt