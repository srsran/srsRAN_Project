/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
    unsigned nof_cells;
    bool     is_rt_mode_enabled;
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
  /// Low priority CPU bitmasks.
  os_sched_affinity_config low_prio_sched_config;
  /// PCAP configuration.
  pcap_config pcap_cfg;
  /// GTPU queue size.
  unsigned gtpu_queue_size;
  /// Vector of affinities mask indexed by cell.
  std::vector<std::vector<os_sched_affinity_config>> config_affinities;
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
