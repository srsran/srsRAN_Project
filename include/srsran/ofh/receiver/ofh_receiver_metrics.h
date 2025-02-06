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

/// Open Fronthaul sector metrics.
struct receiver_metrics {
  /// Number of OFH messages received on time.
  unsigned nof_on_time_messages;
  /// Number of OFH messages received early.
  unsigned nof_early_messages;
  /// Number of OFH messages received late.
  unsigned nof_late_messages;
};

} // namespace ofh
} // namespace srsran
