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

#include "srsran/support/timers.h"

namespace srsran::srs_cu_up {

/// Configuration for E1AP CU-UP.
struct e1ap_configuration {
  /// Whether to enable JSON logging of E1AP Tx and Rx messages.
  bool           json_log_enabled = false;
  timer_duration metrics_period{0};
};

} // namespace srsran::srs_cu_up
