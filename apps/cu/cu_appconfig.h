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

#include "apps/services/os_sched_affinity_manager.h"
#include "srsran/support/executors/unique_thread.h"
#include <optional>
#include <string>

namespace srsran {

/// Configuration of logging functionalities.
struct log_appconfig {
  /// Path to log file or "stdout" to print to console.
  std::string filename = "/tmp/cu.log";
  /// Default log level for all layers.
  std::string all_level = "warning";
  /// Generic log level assigned to library components without layer-specific level.
  std::string lib_level     = "warning";
  std::string e2ap_level    = "warning";
  std::string config_level  = "none";
  std::string metrics_level = "none";
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string tracing_filename;
};

/// Configuration of packet captures.
struct cu_cp_pcap_appconfig {
  struct {
    std::string filename = "/tmp/cu_ngap.pcap";
    bool        enabled  = false;
  } ngap;
  struct {
    std::string filename = "/tmp/cu_e1ap.pcap";
    bool        enabled  = false;
  } e1ap;
  struct {
    std::string filename = "/tmp/cu_f1ap.pcap";
    bool        enabled  = false;
  } f1ap;
  struct {
    std::string filename = "/tmp/cu_e2ap.pcap";
    bool        enabled  = false;
  } e2ap;
  struct {
    std::string filename = "/tmp/cu_gtpu.pcap";
    bool        enabled  = false;
  } gtpu;
};

struct cu_up_pcap_appconfig {
  struct {
    std::string filename = "/tmp/cu_gtpu.pcap";
    bool        enabled  = false;
  } gtpu;
};

/// CPU affinities configuration for the gNB app.
struct cpu_affinities_appconfig {
  /// CPUs isolation.
  std::optional<os_sched_affinity_bitmask> isolated_cpus;
  /// Low priority workers CPU affinity mask.
  os_sched_affinity_config low_priority_cpu_cfg = {sched_affinity_mask_types::low_priority,
                                                   {},
                                                   sched_affinity_mask_policy::mask};
};

/// Non real time thread configuration for the gNB.
struct non_rt_threads_appconfig {
  /// Number of non real time threads for processing of CP and UP data in the upper layers
  unsigned nof_non_rt_threads = 4;
};

/// Expert threads configuration of the CU app.
struct expert_threads_appconfig {
  /// Non real time thread configuration of the gNB app.
  non_rt_threads_appconfig non_rt_threads;
};

/// Expert configuration of the gNB app.
struct expert_execution_appconfig {
  /// gNB CPU affinities.
  cpu_affinities_appconfig affinities;
  /// Expert thread configuration of the gNB app.
  expert_threads_appconfig threads;
};

/// Monolithic gnb application configuration.
struct cu_appconfig {
  /// Logging configuration.
  log_appconfig log_cfg;

  /// PCAP configuration.
  cu_cp_pcap_appconfig cu_cp_pcap_cfg;
  cu_up_pcap_appconfig cu_up_pcap_cfg;

  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// TODO fill in the rest of the configuration
};

} // namespace srsran
