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

#include "srsran/support/engineering_notation.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"
#include <array>
#include <cstdint>

namespace srsran {

/// Container to hold TX/RX metrics
struct e1ap_cu_up_metrics_container {
  uint32_t nof_successful_bearer_context_setup        = 0;
  uint32_t nof_successful_bearer_context_modification = 0;
  uint32_t nof_bearer_context_release                 = 0;

  static constexpr uint32_t nof_msec_per_bin  = 100;
  static constexpr uint32_t latency_hist_bins = 8;

  /// Release latency statistics
  std::chrono::microseconds               sum_release_latency{0};
  std::array<uint32_t, latency_hist_bins> release_latency_hist;
  std::chrono::microseconds               max_release_latency;
};

inline std::string format_e1ap_cu_up_metrics(timer_duration metrics_period, const e1ap_cu_up_metrics_container& m)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "num_success_ctx_setup={} num_success_ctx_modification={} num_ctx_releases={}",
                 m.nof_successful_bearer_context_setup,
                 m.nof_successful_bearer_context_modification,
                 m.nof_bearer_context_release);

  fmt::format_to(
      std::back_inserter(buffer),
      " release_latency_avg={}",
      m.nof_bearer_context_release == 0
          ? "NaN"
          : float_to_eng_string(
                static_cast<float>(m.sum_release_latency.count() * 1e-6) / m.nof_bearer_context_release, 1, false));
  fmt::format_to(std::back_inserter(buffer), " release_latency_hist=[");
  for (unsigned i = 0; i < e1ap_cu_up_metrics_container::latency_hist_bins; i++) {
    fmt::format_to(std::back_inserter(buffer), " {}", float_to_eng_string(m.release_latency_hist[i], 1, false));
  }
  fmt::format_to(std::back_inserter(buffer), "] max_release_latency={}", m.max_release_latency);
  return to_c_str(buffer);
}

} // namespace srsran
