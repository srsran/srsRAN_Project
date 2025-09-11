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

#include "srsran/adt/expected.h"
#include "srsran/support/format/fmt_basic_parser.h"
#include <chrono>

namespace srsran {
namespace resource_usage {

/// Difference between two resource_usage::snapshot points for a given thread.
struct diff {
  /// Number of voluntary context switches between the two snapshots.
  int vol_ctxt_switch_count{0};
  /// Number of involuntary context switches between the two snapshots.
  int invol_ctxt_switch_count{0};
  /// Time spent in user mode between two snapshots.
  std::chrono::microseconds user_time{0};
  /// Time spent in kernel mode between two snapshots.
  std::chrono::microseconds sys_time{0};

  diff& operator+=(const diff& rhs)
  {
    vol_ctxt_switch_count += rhs.vol_ctxt_switch_count;
    invol_ctxt_switch_count += rhs.invol_ctxt_switch_count;
    user_time += rhs.user_time;
    sys_time += rhs.sys_time;
    return *this;
  }
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
