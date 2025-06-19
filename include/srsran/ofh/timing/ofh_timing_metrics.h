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

namespace srsran {
namespace ofh {

/// Open Fronthaul timing metrics.
struct timing_metrics {
  unsigned nof_skipped_symbols;
  unsigned max_nof_continuous_skipped_symbol;
};

} // namespace ofh
} // namespace srsran
