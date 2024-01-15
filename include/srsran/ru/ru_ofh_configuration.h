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

#include "srsran/adt/optional.h"
#include "srsran/ofh/ofh_sector_config.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class ru_timing_notifier;
class ru_uplink_plane_rx_symbol_notifier;
class task_executor;

/// Radio Unit sector configuration for the Open Fronthaul implementation.
struct ru_ofh_sector_configuration {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// Cell channel bandwidth.
  bs_channel_bandwidth_fr1 bw;
  /// \brief RU operating bandwidth.
  ///
  /// Set this option when the operating bandwidth of the RU is larger than the configured bandwidth of the cell.
  optional<bs_channel_bandwidth_fr1> ru_operating_bw;

  /// DU transmission window timing parameters.
  ofh::du_tx_window_timing_parameters tx_window_timing_params;
  /// Reception window timing parameters.
  ofh::du_rx_window_timing_parameters rx_window_timing_params;

  /// Enables the Control-Plane PRACH message signalling.
  bool is_prach_control_plane_enabled = false;
  /// \brief Downlink broadcast flag.
  ///
  /// If enabled, broadcasts the contents of a single antenna port to all downlink RU eAxCs.
  bool is_downlink_broadcast_enabled = false;
  /// If set to true, the payload size encoded in a eCPRI header is ignored.
  bool ignore_ecpri_payload_size_field = false;
  /// If set to true, the sequence id encoded in a eCPRI packet is ignored.
  bool ignore_ecpri_seq_id_field = false;
  /// Uplink compression parameters.
  ofh::ru_compression_params ul_compression_params;
  /// Downlink compression parameters.
  ofh::ru_compression_params dl_compression_params;
  /// PRACH compression parameters.
  ofh::ru_compression_params prach_compression_params;
  /// Downlink static compression header flag.
  bool is_downlink_static_comp_hdr_enabled = true;
  /// Uplink static compression header flag.
  bool is_uplink_static_comp_hdr_enabled = true;
  /// IQ data scaling to be applied prior to Downlink data compression.
  float iq_scaling;

  /// Ethernet interface name.
  std::string interface;
  /// Promiscuous mode flag.
  bool is_promiscuous_mode_enabled;
  /// MTU size.
  units::bytes mtu_size;
  /// Destination MAC address, corresponds to Radio Unit MAC address.
  ether::mac_address mac_dst_address;
  /// Source MAC address, corresponds to Distributed Unit MAC address.
  ether::mac_address mac_src_address;
  /// Tag control information field.
  uint16_t tci;

  /// PRACH eAxC.
  static_vector<unsigned, ofh::MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Downlink eAxCs.
  static_vector<unsigned, ofh::MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  /// Uplink eAxCs.
  static_vector<unsigned, ofh::MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// Number of reception antennas.
  unsigned nof_antennas_ul;
  /// Optional TDD configuration.
  optional<tdd_ul_dl_config_common> tdd_config;
};

/// Radio Unit configuration for the Open Fronthaul implementation.
struct ru_ofh_configuration {
  /// Individual Open Fronthaul sector configurations.
  std::vector<ru_ofh_sector_configuration> sector_configs;

  /// \brief Number of slots the timing handler is notified in advance of the transmission time.
  ///
  /// Sets the maximum allowed processing delay in slots.
  unsigned max_processing_delay_slots;
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta;
  /// Downlink processing time in microseconds.
  std::chrono::microseconds dl_processing_time;

  /// Indicates if DPDK should be used by the underlying implementation.
  bool uses_dpdk;
};

/// Radio Unit sector dependencies for the Open Fronthaul implementation.
struct ru_ofh_sector_dependencies {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// Downlink task executor.
  task_executor* downlink_executor;
  /// Receiver task executor.
  task_executor* receiver_executor = nullptr;
  /// Transmitter task executor.
  task_executor* transmitter_executor = nullptr;
  /// Ethernet gateway.
  std::unique_ptr<ether::gateway> eth_gateway;
};

/// Radio Unit dependencies for the Open Fronthaul implementation.
struct ru_ofh_dependencies {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// Radio Unit timing notifier.
  ru_timing_notifier* timing_notifier = nullptr;
  /// Radio Unit received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier* rx_symbol_notifier = nullptr;
  /// Realtime timing task executor.
  task_executor* rt_timing_executor = nullptr;

  /// Individual Open Fronthaul sector dependencies.
  std::vector<ru_ofh_sector_dependencies> sector_dependencies;
};

/// Returns true if the given Open Fronthaul configuration is valid, otherwise false.
bool is_valid_ru_ofh_config(const ru_ofh_configuration& config);

} // namespace srsran
