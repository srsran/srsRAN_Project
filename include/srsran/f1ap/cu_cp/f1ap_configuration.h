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

#include <chrono>

namespace srsran {
namespace srs_cu_cp {

/// Configuration for F1AP CU-CP.
struct f1ap_configuration {
  /// Timeout for UE context setup procedure (Implementation-defined).
  std::chrono::milliseconds ue_context_setup_timeout = std::chrono::milliseconds(1000);
};

} // namespace srs_cu_cp
} // namespace srsran