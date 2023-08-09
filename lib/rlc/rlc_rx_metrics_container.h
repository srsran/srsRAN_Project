/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/rlc/rlc_rx_metrics.h"
#include "srsran/srslog/srslog.h"
#include <mutex>

namespace srsran {

class rlc_rx_metrics_container
{
  rlc_rx_metrics metrics = {};
  std::mutex     metrics_mutex;

public:
  void metrics_set_mode(rlc_mode mode)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.mode = mode;
  }

  void metrics_add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void metrics_add_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_pdus += num_pdus_;
    metrics.num_pdu_bytes += num_pdu_bytes_;
  }

  void metrics_add_lost_pdus(uint32_t num_pdus_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_lost_pdus += num_pdus_;
  }

  void metrics_add_malformed_pdus(uint32_t num_pdus_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_malformed_pdus += num_pdus_;
  }

  /// RLC AM specific metrics
  void metrics_add_ctrl_pdus(uint32_t num_ctrl_, uint32_t num_ctrl_pdu_bytes_)
  {
    srsran_assert(metrics.mode == rlc_mode::am, "Wrong mode for AM metrics.");
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.mode_specific.am.num_ctrl_pdus += num_ctrl_;
    metrics.mode_specific.am.num_ctrl_pdu_bytes += num_ctrl_pdu_bytes_;
  }

  rlc_rx_metrics get_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    return metrics;
  }

  rlc_rx_metrics get_and_reset_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    rlc_rx_metrics              ret = metrics;
    metrics                         = {};
    metrics.mode                    = ret.mode;
    return ret;
  }

  void reset_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    rlc_mode                    tmp_mode = metrics.mode;
    metrics                              = {};
    metrics.mode                         = tmp_mode;
  }
};
} // namespace srsran
