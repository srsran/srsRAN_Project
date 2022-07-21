/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <mutex>

namespace srsgnb {

struct rlc_bearer_tx_metrics_container {
  // SDU metrics
  uint32_t num_sdus;         ///< Number of SDUs
  size_t   num_sdu_bytes;    ///< Number of SDU bytes
  uint32_t num_dropped_sdus; ///< Number of dropped SDUs (full queue)

  // PDU metrics
  uint32_t num_pdus;      ///< Number of PDUs
  size_t   num_pdu_bytes; ///< Number of PDU bytes
};

class rlc_bearer_tx_metrics
{
  rlc_bearer_tx_metrics_container metrics = {};
  std::mutex                      metrics_mutex;

public:
  void metrics_add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void metrics_add_lost_sdus(uint32_t num_sdus_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_dropped_sdus += num_sdus_;
  }

  void metrics_add_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_pdus += num_pdus_;
    metrics.num_pdu_bytes += num_pdu_bytes_;
  }

  rlc_bearer_tx_metrics_container get_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    return metrics;
  }

  rlc_bearer_tx_metrics_container get_and_reset_metrics()
  {
    std::lock_guard<std::mutex>     lock(metrics_mutex);
    rlc_bearer_tx_metrics_container ret = metrics;
    metrics                             = {};
    return ret;
  }

  void reset_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics = {};
  }
};

struct rlc_bearer_rx_metrics_container {
  // SDU metrics
  uint32_t num_sdus;      ///< Number of SDUs
  size_t   num_sdu_bytes; ///< Number of SDU bytes

  // PDU metrics
  uint32_t num_pdus;           ///< Number of PDUs
  size_t   num_pdu_bytes;      ///< Number of PDU bytes
  uint32_t num_lost_pdus;      ///< Number of dropped PDUs (reassembly timeout expiry or out of rx window)
  uint32_t num_malformed_pdus; ///< Number of malformed PDUs
};

class rlc_bearer_rx_metrics
{
  rlc_bearer_rx_metrics_container metrics = {};
  std::mutex                      metrics_mutex;

public:
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

  rlc_bearer_rx_metrics_container get_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    return metrics;
  }

  rlc_bearer_rx_metrics_container get_and_reset_metrics()
  {
    std::lock_guard<std::mutex>     lock(metrics_mutex);
    rlc_bearer_rx_metrics_container ret = metrics;
    metrics                             = {};
    return ret;
  }

  void reset_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics = {};
  }
};

struct rlc_bearer_metrics_container {
  rlc_bearer_tx_metrics_container tx;
  rlc_bearer_rx_metrics_container rx;
};

} // namespace srsgnb
