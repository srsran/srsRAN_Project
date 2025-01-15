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

#include <chrono>

namespace srsran {
namespace srs_cu_cp {

/// Configuration for F1AP CU-CP.
struct f1ap_configuration {
  /// Timeout for waiting for the DU response during F1AP procedures (Implementation-defined).
  std::chrono::milliseconds proc_timeout{1000};
  /// Whether to enable JSON logging of F1AP Tx and Rx messages.
  bool json_log_enabled = false;
};

} // namespace srs_cu_cp
} // namespace srsran