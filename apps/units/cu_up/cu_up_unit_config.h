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

#include "apps/units/cu_up/cu_up_unit_pcap_config.h"
#include "cu_up_unit_logger_config.h"
#include "srsran/ran/five_qi.h"

namespace srsran {

/// Metrics configuration.
struct cu_up_unit_metrics_config {
  /// Statistics report period in seconds.
  unsigned cu_up_statistics_report_period = 1;
  struct pdcp_metrics {
    unsigned report_period = 0; // PDCP report period in ms
  } pdcp;
};

struct cu_up_unit_upf_config {
  std::string bind_addr         = "127.0.0.1";
  std::string n3_bind_addr      = "auto";
  std::string n3_bind_interface = "auto";
  std::string n3_ext_addr       = "auto";
  int         udp_rx_max_msgs   = 256;
  bool        no_core           = false;
};

struct cu_up_nru_appconfig {
  std::string bind_addr       = "127.0.10.1"; // Bind address used by the F1-U interface
  int         udp_rx_max_msgs = 256; // Max number of UDP packets received by a single syscall on the F1-U interface.
};

/// QoS configuration.
struct cu_up_unit_qos_config {
  five_qi_t   five_qi       = uint_to_five_qi(9);
  std::string mode          = "am";
  unsigned    rlc_sdu_queue = 4096;
};

/// CU-UP application unit configuration.
struct cu_up_unit_config {
  unsigned gtpu_queue_size          = 2048;
  unsigned gtpu_reordering_timer_ms = 0;
  bool     warn_on_drop             = false;
  /// UPF configuration.
  cu_up_unit_upf_config upf_cfg;
  /// Metrics.
  cu_up_unit_metrics_config metrics;
  /// Loggers.
  cu_up_unit_logger_config loggers;
  /// PCAPs.
  cu_up_unit_pcap_config pcap_cfg;
  /// NR-U
  cu_up_nru_appconfig nru_cfg;
  /// QoS configuration.
  std::vector<cu_up_unit_qos_config> qos_cfg;
};

} // namespace srsran
