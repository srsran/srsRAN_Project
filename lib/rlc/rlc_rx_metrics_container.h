/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/rlc/rlc_mode.h"
#include "srsran/rlc/rlc_rx_metrics.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class rlc_rx_metrics_container
{
  rlc_rx_metrics metrics = {};
  bool           enabled = false;

public:
  rlc_rx_metrics_container(bool enabled_) : enabled(enabled_) {}

  void metrics_set_mode(rlc_mode mode)
  {
    if (not enabled) {
      return;
    }
    switch (mode) {
      case rlc_mode::tm:
        metrics.mode_specific = rlc_tm_rx_metrics{};
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
      case rlc_mode::um_unidir_ul:
        metrics.mode_specific = rlc_um_rx_metrics{};
        break;
      case rlc_mode::am:
        metrics.mode_specific = rlc_am_rx_metrics{};
        break;
      default:
        break;
    }
  }

  void metrics_add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void metrics_add_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_pdus += num_pdus_;
    metrics.num_pdu_bytes += num_pdu_bytes_;
  }

  void metrics_add_lost_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_lost_pdus += num_pdus_;
  }

  void metrics_add_malformed_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_malformed_pdus += num_pdus_;
  }

  void metrics_add_sdu_latency(uint32_t sdu_latency_us_)
  {
    if (not enabled) {
      return;
    }
    metrics.sdu_latency_us += sdu_latency_us_;
  }

  /// RLC AM specific metrics
  void metrics_add_ctrl_pdus(uint32_t num_ctrl_, uint32_t num_ctrl_pdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_am_rx_metrics>(metrics.mode_specific), "Wrong mode for AM metrics.");
    auto& am = std::get<rlc_am_rx_metrics>(metrics.mode_specific);
    am.num_ctrl_pdus += num_ctrl_;
    am.num_ctrl_pdu_bytes += num_ctrl_pdu_bytes_;
  }

  rlc_rx_metrics get_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    return metrics.get();
  }

  rlc_rx_metrics get_and_reset_metrics()
  {
    srsran_assert(enabled, "Trying to get and reset metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    rlc_rx_metrics ret = metrics.get();
    reset_metrics();
    return ret;
  }

  void reset_metrics()
  {
    srsran_assert(enabled, "Trying to reset metrics, but metrics are disabled.");
    if (not enabled) {
      return;
    }
    metrics.reset();
  }
};
} // namespace srsran
