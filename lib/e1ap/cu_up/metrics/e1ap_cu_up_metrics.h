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

#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"
#include <cstdint>

namespace srsran {

/// Container to hold TX/RX metrics
struct e1ap_cu_up_metrics_container {
  uint32_t nof_successful_bearer_context_setup        = 0;
  uint32_t nof_successful_bearer_context_modification = 0;
  uint32_t nof_bearer_context_release                 = 0;
};

inline std::string format_e1ap_cu_up_metrics(timer_duration metrics_period, const e1ap_cu_up_metrics_container& m)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "num_success_ctx_setup={} num_success_ctx_modification={} num_ctx_releases={}",
                 m.nof_successful_bearer_context_setup,
                 m.nof_successful_bearer_context_modification,
                 m.nof_bearer_context_release);
  return to_c_str(buffer);
}

} // namespace srsran
