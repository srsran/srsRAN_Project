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

#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/network/udp_appconfig.h"
#include "apps/units/o_cu_up/cu_up/cu_up_unit_pcap_config.h"
#include "cu_up_unit_logger_config.h"
#include "srsran/ran/gnb_cu_up_id.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/qos/five_qi.h"

namespace srsran {

/// Configuration to enable/disable metrics per layer.
struct cu_up_unit_metrics_layer_config {
  bool enable_pdcp           = false;
  bool enable_e1ap           = false;
  bool enable_cu_up_executor = false;

  /// Returns true if one or more layers are enabled, false otherwise.
  bool are_metrics_enabled() const { return enable_pdcp || enable_e1ap; }
};

/// Metrics configuration.
struct cu_up_unit_metrics_config {
  /// CU-UP statistics report period in milliseconds.
  unsigned                        cu_up_report_period = 1000;
  app_helpers::metrics_config     common_metrics_cfg;
  cu_up_unit_metrics_layer_config layers_cfg;
};

struct cu_up_unit_ngu_socket_config {
  std::string   bind_addr      = "127.0.0.1";
  std::string   bind_interface = "auto";
  std::string   ext_addr       = "auto";
  udp_appconfig udp_config     = {};
};

/// GPTU parameters.
struct cu_up_unit_ngu_gtpu_config {
  unsigned                  gtpu_queue_size          = 2046;
  unsigned                  gtpu_batch_size          = 256;
  unsigned                  gtpu_reordering_timer_ms = 0;
  std::chrono::milliseconds rate_limiter_period{100};
  bool                      ignore_ue_ambr = true;
};

struct cu_up_unit_ngu_config {
  bool                                      no_core = false;
  std::vector<cu_up_unit_ngu_socket_config> ngu_socket_cfg;
  cu_up_unit_ngu_gtpu_config                gtpu_cfg;
};

/// F1-U configuration at CU_UP side
struct cu_cp_unit_f1u_config {
  uint32_t queue_size = 8192; ///< Queue size for F1-U PDUs
  uint32_t batch_size = 256;  ///< Batch size for F1-U PDUs
  int32_t  t_notify   = 10;   ///< Maximum backoff time for discard notifications from CU_UP to DU (ms)
};

/// QoS configuration.
struct cu_up_unit_qos_config {
  five_qi_t             five_qi = uint_to_five_qi(9);
  std::string           mode    = "am";
  cu_cp_unit_f1u_config f1u_cu_up;
};

struct cu_up_unit_test_mode_config {
  bool                      enabled           = false;
  bool                      integrity_enabled = true;
  bool                      ciphering_enabled = true;
  uint16_t                  nea_algo          = 2;
  uint16_t                  nia_algo          = 2;
  uint64_t                  ue_ambr           = 40000000000; // 40 gbps
  std::chrono::milliseconds attach_detach_period{0}; // Period for attaching detaching tests. 0 means always attached.
};

struct cu_up_unit_execution_config {
  uint32_t dl_ue_executor_queue_size   = 8192;
  uint32_t ul_ue_executor_queue_size   = 8192;
  uint32_t ctrl_ue_executor_queue_size = 8192;
  unsigned strand_batch_size           = 256;
  bool     executor_tracing_enable     = false;
};

/// CU-UP application unit configuration.
struct cu_up_unit_config {
  /// gNB identifier.
  gnb_id_t gnb_id = {411, 22};
  /// CU-UP identifier.
  gnb_cu_up_id_t gnb_cu_up_id = gnb_cu_up_id_t::min;
  /// CU-UP warn if PDUs are dropped.
  bool warn_on_drop = false;
  /// NG-U configuration.
  cu_up_unit_ngu_config ngu_cfg;
  /// Execution configuration
  cu_up_unit_execution_config exec_cfg;
  /// Metrics.
  cu_up_unit_metrics_config metrics;
  /// Loggers.
  cu_up_unit_logger_config loggers;
  /// PCAPs.
  cu_up_unit_pcap_config pcap_cfg;
  /// QoS configuration.
  std::vector<cu_up_unit_qos_config> qos_cfg;
  /// Test mode.
  cu_up_unit_test_mode_config test_mode_cfg;
};

} // namespace srsran
