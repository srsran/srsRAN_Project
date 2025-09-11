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

#include "srsran/adt/static_vector.h"
#include "srsran/ofh/compression/compression_params.h"
#include "srsran/ofh/ethernet/ethernet_mac_address.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_transmitter.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/ofh/receiver/ofh_receiver_configuration.h"
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srslog/srslog.h"
#include <string>

namespace srsran {

class task_executor;

namespace ofh {

class error_notifier;

/// Open Fronthaul sector configuration.
struct sector_configuration {
  /// Radio sector identifier.
  unsigned sector_id;
  /// Ethernet interface name or identifier.
  std::string interface;
  /// Promiscuous mode flag.
  bool is_promiscuous_mode_enabled;
  /// Ethernet link status checking flag.
  bool is_link_status_check_enabled;
  /// MTU size.
  units::bytes mtu_size;
  /// Destination MAC address, corresponds to the Radio Unit MAC address.
  ether::mac_address mac_dst_address;
  /// Source MAC address, corresponds to the Distributed Unit MAC address.
  ether::mac_address mac_src_address;
  /// Tag control information field for C-Plane.
  std::optional<uint16_t> tci_cp;
  /// Tag control information field for U-Plane.
  std::optional<uint16_t> tci_up;

  /// DU transmission window timing parameters.
  tx_window_timing_parameters tx_window_timing_params;
  /// Reception window timing parameters.
  rx_window_timing_parameters rx_window_timing_params;

  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// Cell channel bandwidth.
  bs_channel_bandwidth bw;
  /// \brief RU operating bandwidth.
  ///
  /// Set this option when the operating bandwidth of the RU is larger than the configured bandwidth of the cell.
  bs_channel_bandwidth ru_operating_bw;

  /// PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Downlink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;

  /// Enables the Control-Plane PRACH message signalling.
  bool is_prach_control_plane_enabled = false;
  /// Ignore the start symbol value received in the PRACH U-Plane packets.
  bool ignore_prach_start_symbol = false;
  /// If set to true, the payload size encoded in a eCPRI header is ignored.
  bool ignore_ecpri_payload_size_field = false;
  /// If set to true, the sequence id encoded in a eCPRI packet is ignored.
  bool ignore_ecpri_seq_id_field = false;
  /// If set to true, metrics are enabled in the sector.
  bool are_metrics_enabled = false;
  /// If set to true, logs late events as warnings, otherwise as info.
  bool enable_log_warnings_for_lates = true;
  /// Warn of unreceived Radio Unit frames status.
  warn_unreceived_ru_frames log_unreceived_ru_frames = warn_unreceived_ru_frames::after_traffic_detection;
  /// Uplink compression parameters.
  ru_compression_params ul_compression_params;
  /// Downlink compression parameters.
  ru_compression_params dl_compression_params;
  /// PRACH compression parameters.
  ru_compression_params prach_compression_params;
  /// Downlink static compression header flag.
  bool is_downlink_static_compr_hdr_enabled = true;
  /// Uplink static compression header flag.
  bool is_uplink_static_compr_hdr_enabled = true;
  /// IQ data scaling to be applied prior to Downlink data compression.
  float iq_scaling;
  /// C-Plane PRACH FFT size (to be used in Type 3 messages).
  ofh::cplane_fft_size c_plane_prach_fft_len = ofh::cplane_fft_size::fft_4096;
  /// \brief Number of slots the timing handler is notified in advance of the transmission time.
  ///
  /// Sets the maximum allowed processing delay in slots.
  unsigned max_processing_delay_slots;
  /// Downlink processing time in microseconds.
  std::chrono::microseconds dl_processing_time;
  /// Uplink request processing time in microseconds.
  std::chrono::microseconds ul_processing_time;
  /// Number of reception antennas.
  unsigned nof_antennas_ul;

  /// Indicates if DPDK should be used by the underlying implementation.
  bool uses_dpdk;
  /// Optional TDD configuration.
  std::optional<tdd_ul_dl_config_common> tdd_config;
};

/// Open Fronthaul sector dependencies.
struct sector_dependencies {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// Error notifier.
  error_notifier* err_notifier = nullptr;
  /// Downlink task executor.
  task_executor* downlink_executor = nullptr;
  /// Message transmitter and receiver task executor.
  task_executor* txrx_executor = nullptr;
  /// Uplink task executor.
  task_executor* uplink_executor = nullptr;
  /// User-Plane received symbol notifier.
  uplane_rx_symbol_notifier* notifier = nullptr;
  /// Optional Ethernet transmitter.
  std::optional<std::unique_ptr<ether::transmitter>> eth_transmitter;
  /// Optional Ethernet receiver.
  std::optional<std::unique_ptr<ether::receiver>> eth_receiver;
};

} // namespace ofh
} // namespace srsran
