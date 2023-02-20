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

#include "srsgnb/rlc/rlc_rx_metrics.h"
#include "srsgnb/srslog/srslog.h"
#include <mutex>

namespace srsran {

class rlc_rx_metrics_container
{
  rlc_rx_metrics metrics = {};
  std::mutex     metrics_mutex;

public:
  void metrics_set_mode(rlc_mode mode) { metrics.mode = mode; }

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
    return ret;
  }

  void reset_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics = {};
  }
};
} // namespace srsran
