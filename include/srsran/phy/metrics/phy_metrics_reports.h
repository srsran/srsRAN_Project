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

#include "srsran/support/units.h"
#include <chrono>
#include <optional>

namespace srsran {

/// Collects LDPC decoder metrics.
struct ldpc_decoder_metrics {
  /// Codeblock size.
  units::bits cb_sz;
  /// Number of iterations.
  unsigned nof_iterations;
  /// Set to true if the CRC matches.
  bool crc_ok;
  /// Elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects PUSCH channel estimator metrics.
struct pusch_channel_estimator_metrics {
  /// PUSCH transmission bandwidth.
  unsigned nof_prb;
  /// Elapsed time.
  std::chrono::nanoseconds elapsed;
};

/// Collects PUSCH processor metrics.
struct pusch_processor_metrics {
  /// Codeblock size.
  units::bytes tbs;
  /// Set to true if the CRC matches.
  bool crc_ok;
  /// Elapsed time between the start of the processing and the return.
  std::chrono::nanoseconds elapsed_return;
  /// Elapsed time between the start of the processing and the completion of the processing.
  std::chrono::nanoseconds elapsed_data;
  /// Optional elapsed time between the start of the processing and the UCI notification. Set to default value if UCI
  /// is not present.
  std::optional<std::chrono::nanoseconds> elapsed_uci;
};

} // namespace srsran
