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

#include "os_sched_affinity_manager.h"

namespace srsran {

class timer_manager;

/// Worker manager configuration.
struct worker_manager_config {
  /// RU OFH worker configuration.
  struct ru_ofh_config {
    bool is_downlink_parallelized;
    /// Number of downlink antennas indexed by cell.
    std::vector<unsigned> nof_downlink_antennas;
    /// RU timing CPU affinity mask.
    os_sched_affinity_bitmask ru_timing_cpu;
    /// Vector of affinities for the txrx workers.
    std::vector<os_sched_affinity_bitmask> txrx_affinities;
  };

  /// RU SDR worker configuration.
  struct ru_sdr_config {
    /// Lower physical layer thread profiles.
    enum class lower_phy_thread_profile {
      /// Same task worker as the rest of the PHY (ZMQ only).
      blocking = 0,
      /// Single task worker for all the lower physical layer task executors.
      single,
      /// Two task workers - one for the downlink and one for the uplink.
      dual,
      /// Dedicated task workers for each of the subtasks (downlink processing, uplink processing, reception and
      /// transmission).
      quad
    };

    lower_phy_thread_profile profile;
    unsigned                 nof_cells;
  };

  /// RU dummy worker configuration.
  struct ru_dummy_config {};

  /// FAPI worker configuration.
  struct fapi_config {
    unsigned nof_cells;
  };

  /// DU low worker configuration.
  struct du_low_config {
    bool     is_blocking_mode_active;
    unsigned nof_ul_threads;
    unsigned nof_dl_threads;
    unsigned nof_pusch_decoder_threads;
    unsigned nof_cells;
  };

  /// DU high worker configuration.
  struct du_high_config {
    /// DU-high UE data related tasks queue size.
    unsigned ue_data_tasks_queue_size;
    /// DU high number of cells.
    unsigned nof_cells;
    /// Real-time mode enabled flag.
    bool is_rt_mode_enabled;
    /// Whether to log performance metrics for the DU-high executors.
    std::optional<std::chrono::milliseconds> metrics_period;
    /// Whether to enable task tracing.
    bool executor_tracing_enable;
  };

  // CU-UP worker configuration
  struct cu_up_config {
    unsigned max_nof_ue_strands = 16;
    /// UE task queue size.
    uint32_t dl_ue_executor_queue_size   = 2048;
    uint32_t ul_ue_executor_queue_size   = 2048;
    uint32_t ctrl_ue_executor_queue_size = 2048;
    /// Maximum batch size used in CU-UP strands.
    unsigned strand_batch_size = 256;
    /// Wether to offload socket TX to a dedicated strand.
    bool dedicated_io_ul_strand = true;
    /// Whether to enable task tracing.
    bool executor_tracing_enable = false;
  };

  /// PCAP worker configuration.
  struct pcap_config {
    bool is_f1ap_enabled = false;
    bool is_ngap_enabled = false;
    bool is_e1ap_enabled = false;
    bool is_e2ap_enabled = false;
    bool is_n3_enabled   = false;
    bool is_f1u_enabled  = false;
    bool is_mac_enabled  = false;
    bool is_rlc_enabled  = false;
  };

  /// Number of low priority threads.
  unsigned nof_low_prio_threads;
  /// Low priority task worker queue size.
  unsigned low_prio_task_queue_size;
  /// Low priority CPU bitmasks.
  os_sched_affinity_config low_prio_sched_config;
  /// PCAP configuration.
  pcap_config pcap_cfg;
  /// Timer config.
  timer_manager* app_timers = nullptr;
  /// Vector of affinities mask indexed by cell.
  std::vector<std::vector<os_sched_affinity_config>> config_affinities;
  /// Split 6 enabled flag.
  bool is_split6_enabled = false;
  /// CU-UP configuration.
  std::optional<cu_up_config> cu_up_cfg;
  /// DU high configuration.
  std::optional<du_high_config> du_hi_cfg;
  /// FAPI configuration.
  std::optional<fapi_config> fapi_cfg;
  /// DU low configuration
  std::optional<du_low_config> du_low_cfg;
  /// RU SDR configuration.
  std::optional<ru_sdr_config> ru_sdr_cfg;
  /// RU OFH configuration.
  std::optional<ru_ofh_config> ru_ofh_cfg;
  /// RU dummy configuration.
  std::optional<ru_dummy_config> ru_dummy_cfg;
};

} // namespace srsran
