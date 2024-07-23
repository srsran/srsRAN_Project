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

#include "apps/services/os_sched_affinity_manager.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/support/units.h"
#include <chrono>
#include <string>
#include <vector>

namespace srsran {

/// gNB app Open Fronthaul base cell configuration.
struct ru_ofh_unit_base_cell_config {
  /// \brief RU operating bandwidth.
  ///
  /// Set this option when the operating bandwidth of the RU is larger than the configured bandwidth of the cell.
  std::optional<bs_channel_bandwidth_fr1> ru_operating_bw;
  /// T1a maximum parameter for downlink Control-Plane in microseconds.
  std::chrono::microseconds T1a_max_cp_dl{500};
  /// T1a minimum parameter for downlink Control-Plane in microseconds.
  std::chrono::microseconds T1a_min_cp_dl{258};
  /// T1a maximum parameter for uplink Control-Plane in microseconds.
  std::chrono::microseconds T1a_max_cp_ul{500};
  /// T1a minimum parameter for uplink Control-Plane in microseconds.
  std::chrono::microseconds T1a_min_cp_ul{285};
  /// T1a maximum parameter for downlink User-Plane in microseconds.
  std::chrono::microseconds T1a_max_up{300};
  /// T1a minimum parameter for downlink User-Plane in microseconds.
  std::chrono::microseconds T1a_min_up{85};
  /// Ta4 maximum parameter for uplink User-Plane in microseconds.
  std::chrono::microseconds Ta4_max{300};
  /// Ta4 minimum parameter for uplink User-Plane in microseconds.
  std::chrono::microseconds Ta4_min{85};
  /// Enables the Control-Plane PRACH message signalling.
  bool is_prach_control_plane_enabled = true;
  /// \brief Downlink broadcast flag.
  ///
  /// If enabled, broadcasts the contents of a single antenna port to all downlink RU eAxCs.
  bool is_downlink_broadcast_enabled = false;
  /// If set to true, the payload size encoded in a eCPRI header is ignored.
  bool ignore_ecpri_payload_size_field = false;
  /// If set to true, the sequence id encoded in a eCPRI packet is ignored.
  bool ignore_ecpri_seq_id_field = false;
  /// If set to true, warn of unreceived Radio Unit frames.
  bool warn_unreceived_ru_frames = true;
  /// Uplink compression method.
  std::string compression_method_ul = "bfp";
  /// Uplink compression bitwidth.
  unsigned compression_bitwidth_ul = 9;
  /// Downlink compression method.
  std::string compression_method_dl = "bfp";
  /// Downlink compression bitwidth.
  unsigned compression_bitwidth_dl = 9;
  /// PRACH compression method.
  std::string compression_method_prach = "bfp";
  /// PRACH compression bitwidth.
  unsigned compression_bitwidth_prach = 9;
  /// Downlink static compression header flag.
  bool is_downlink_static_comp_hdr_enabled = true;
  /// Uplink static compression header flag.
  bool is_uplink_static_comp_hdr_enabled = true;
  /// IQ data scaling to be applied prior to Downlink data compression.
  float iq_scaling = 0.35F;
};

/// gNB app Open Fronthaul cell configuration.
struct ru_ofh_unit_cell_config {
  /// Base cell configuration.
  ru_ofh_unit_base_cell_config cell;
  /// Ethernet network interface name or identifier.
  std::string network_interface = "enp1s0f0";
  /// Promiscuous mode flag.
  bool enable_promiscuous_mode = false;
  /// MTU size.
  units::bytes mtu_size{9000};
  /// Radio Unit MAC address.
  std::string ru_mac_address = "70:b3:d5:e1:5b:06";
  /// Distributed Unit MAC address.
  std::string du_mac_address = "00:11:22:33:00:77";
  /// V-LAN Tag control information field for C-Plane.
  uint16_t vlan_tag_cp = 1U;
  /// V-LAN Tag control information field for U-Plane.
  uint16_t vlan_tag_up = 1U;
  /// RU PRACH port.
  std::vector<unsigned> ru_prach_port_id = {4, 5};
  /// RU Downlink port.
  std::vector<unsigned> ru_dl_port_id = {0, 1};
  /// RU Uplink port.
  std::vector<unsigned> ru_ul_port_id = {0, 1};
};

/// RU OFH logging functionalities.
struct ru_ofh_unit_logger_config {
  srslog::basic_levels ofh_level = srslog::basic_levels::warning;
};

/// CPU affinities configuration for the cell.
struct ru_ofh_unit_cpu_affinities_cell_config {
  /// Radio Unit workers CPU affinity mask.
  os_sched_affinity_config ru_cpu_cfg = {sched_affinity_mask_types::ru, {}, sched_affinity_mask_policy::mask};
};

/// Expert threads configuration.
struct ru_ofh_unit_expert_threads_config {
  ru_ofh_unit_expert_threads_config()
  {
    unsigned nof_threads     = cpu_architecture_info::get().get_host_nof_available_cpus();
    is_downlink_parallelized = nof_threads > 3;
  }

  /// Open Fronthaul thread configuration.
  bool is_downlink_parallelized = true;
};

/// Expert configuration.
struct ru_ofh_unit_expert_execution_config {
  /// RU timing thread.
  os_sched_affinity_bitmask ru_timing_cpu;
  /// Expert thread configuration of the Open Fronthaul Radio Unit.
  ru_ofh_unit_expert_threads_config threads;
  /// CPU affinities per cell.
  std::vector<ru_ofh_unit_cpu_affinities_cell_config> cell_affinities = {{}};
};

/// HAL configuration.
struct ru_ofh_unit_hal_config {
  /// EAL configuration arguments.
  std::string eal_args;
};

/// gNB app Open Fronthaul Radio Unit configuration.
struct ru_ofh_unit_config {
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha = 0;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta = 0;
  /// Downlink processing time in microseconds.
  unsigned dl_processing_time = 400U;
  /// Loggers.
  ru_ofh_unit_logger_config loggers;
  /// \brief Individual Open Fronthaul cells configurations.
  ///
  /// \note Add one cell by default.
  std::vector<ru_ofh_unit_cell_config> cells = {{}};
  /// \brief Expert execution parameters for the Open Fronthaul Radio Unit.
  ///
  /// \note Add one cell by default.
  ru_ofh_unit_expert_execution_config expert_execution_cfg;
  /// HAL configuration.
  std::optional<ru_ofh_unit_hal_config> hal_config;
};

/// gNB app Open Fronthaul Radio Unit configuration.
struct ru_ofh_unit_parsed_config {
  /// Open Fronthaul Radio Unit configuration.
  ru_ofh_unit_config config;
  /// Base cell configuration for the Radio Unit.
  ru_ofh_unit_base_cell_config base_cell_cfg;
};

} // namespace srsran
