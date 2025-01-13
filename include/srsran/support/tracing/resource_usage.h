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
  long vol_ctxt_switch_count;
  /// Number of involuntary context switches between the two snapshots.
  long invol_ctxt_switch_count;
};

/// Snapshot of the resource usage statistics of a specific thread at given point in time.
struct snapshot {
  /// Number of voluntary context switches at this point in time.
  long int vol_ctxt_switch_count = 0;
  /// Number of involuntary context switches at this point in time.
  long int invol_ctxt_switch_count = 0;

  constexpr snapshot() {}
  constexpr snapshot(long int vol_cswitch_, long int invol_switch_) :
    vol_ctxt_switch_count(vol_cswitch_), invol_ctxt_switch_count(invol_switch_)
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
