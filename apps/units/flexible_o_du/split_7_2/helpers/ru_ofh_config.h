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
#include "apps/services/worker_manager/os_sched_affinity_manager.h"
#include "srsran/ofh/receiver/ofh_receiver_configuration.h"
#include "srsran/ofh/serdes/ofh_cplane_message_properties.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/support/units.h"
#include <chrono>
#include <string>
#include <variant>
#include <vector>

namespace srsran {

/// Configuration parameters related to the scaling of the IQ symbols in the DL OFH resource grid.
struct ru_ofh_scaling_config {
  /// \brief RU reference level in dBFS.
  ///
  /// This parameter must be set to the magnitude that is required for a single subcarrier to reach the nominal RF power
  /// of the RU. Some RUs specify their IQ reference level in their documentation. It is referenced to the full scale
  /// value of the selected OFH compression scheme.
  ///
  /// \warning Check the RU documentation before settings this parameter, as incorrect values may damage the RU.
  float ru_reference_level_dBFS = -12.0f;
  /// \brief Attenuation of the subcarrier IQ data as a back-off to \ref ru_reference_level_dBFS.
  ///
  /// Set this parameter to the desired RMS power attenuation of the IQ symbols in the resource grid. It acts as a
  /// back-off applied to the RU reference level. Its intended use is to prevent the sum of the power contributions of
  /// each subcarrier in the signal from exceeding the RU reference level. Some RUs specify the expected subcarrier
  /// magnitude in their documentation. If no expected subcarrier magnitude is stated on the RU documentation, it is
  /// recommended to start with a large back-off (e.g., 30 dB), and decrease gradually until the desired RF power is
  /// reached.
  ///
  /// Some RUs that support multiple channel bandwidths normalize their internal gain according to the selected RU
  /// bandwidth. This makes the subcarrier RMS power valid for all supported bandwidth settings. For RUs that don't
  /// normalize based on channel bandwidth, a subcarrier RMS power for a new BW can be derived from a subcarrier RMS
  /// power calibrated for a previous BW by applying the following expression:
  ///
  /// \f$ $$P_{BW2}\text{ [dBFS]}=P_{BW1}\text{ [dBFS]} - 10\log_{10}{\bigg(\frac{BW2}{BW1}\bigg)}\f$
  ///
  /// If no value is specified, then the bandwidth is normalized based on the number of subcarriers, i.e.,
  /// the back-off is set to \f$10\log_{10}(12 N_{RB})}\f$.
  ///
  /// \warning Check the RU documentation before settings this parameter, as incorrect values may damage the RU.
  std::optional<float> subcarrier_rms_backoff_dB;
};

/// Configuration parameters related to the scaling of the IQ symbols in the DL OFH resource grid (legacy version).
struct ru_ofh_legacy_scaling_config {
  /// \brief IQ data scaling to be applied prior to Downlink data compression.
  ///
  /// If configured, it overrides the other power configuration parameters \ref ru_reference_level_dBFS and \ref
  /// subcarrier_rms_backoff_dB. To preserve backwards-compatibility with older configs, the IQ scaling factor applied
  /// to the OFH symbols will include \f$ G_{iq}=\frac{\text{iq\_scaling}}{\sqrt{12 N_{RB}}} \f$.
  ///
  /// \warning Check the RU documentation before settings this parameter, as incorrect values may damage the RU.
  float iq_scaling = 0.1f;
};

/// gNB app Open Fronthaul base cell configuration.
struct ru_ofh_unit_base_cell_config {
  /// \brief RU operating bandwidth.
  ///
  /// Set this option when the operating bandwidth of the RU is larger than the configured bandwidth of the cell.
  std::optional<bs_channel_bandwidth> ru_operating_bw;
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
  std::chrono::microseconds Ta4_max{500};
  /// Ta4 minimum parameter for uplink User-Plane in microseconds.
  std::chrono::microseconds Ta4_min{85};
  /// Enables the Control-Plane PRACH message signalling.
  bool is_prach_control_plane_enabled = true;
  /// Ignore the start symbol value received in the PRACH U-Plane packets.
  bool ignore_prach_start_symbol = false;
  /// If set to true, the payload size encoded in a eCPRI header is ignored.
  bool ignore_ecpri_payload_size_field = false;
  /// If set to true, the sequence id encoded in a eCPRI packet is ignored.
  bool ignore_ecpri_seq_id_field = false;
  /// If set to true, logs late events as warnings, otherwise as info.
  bool enable_log_warnings_for_lates = true;
  /// Warn of unreceived Radio Unit frames status.
  ofh::warn_unreceived_ru_frames log_unreceived_ru_frames = ofh::warn_unreceived_ru_frames::after_traffic_detection;
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
  /// Scaling configuration parameters.
  std::variant<std::monostate, ru_ofh_scaling_config, ru_ofh_legacy_scaling_config> iq_scaling_config;
  /// PRACH FFT size (to be used in the C-plane Type 3 messages).
  ofh::cplane_fft_size c_plane_prach_fft_len = ofh::cplane_fft_size::fft_4096;
};

/// gNB app Open Fronthaul cell configuration.
struct ru_ofh_unit_cell_config {
  /// Base cell configuration.
  ru_ofh_unit_base_cell_config cell;
  /// Ethernet network interface name or identifier.
  std::string network_interface = "enp1s0f0";
  /// Promiscuous mode flag.
  bool enable_promiscuous_mode = false;
  /// Ethernet link status checking flag.
  bool check_link_status = false;
  /// MTU size.
  units::bytes mtu_size{9000};
  /// Radio Unit MAC address.
  std::string ru_mac_address = "70:b3:d5:e1:5b:06";
  /// Distributed Unit MAC address.
  std::string du_mac_address = "00:11:22:33:00:77";
  /// V-LAN Tag control information field for C-Plane.
  std::optional<uint16_t> vlan_tag_cp;
  /// V-LAN Tag control information field for U-Plane.
  std::optional<uint16_t> vlan_tag_up;
  /// RU PRACH port.
  std::vector<unsigned> ru_prach_port_id = {4};
  /// RU Downlink port.
  std::vector<unsigned> ru_dl_port_id = {0};
  /// RU Uplink port.
  std::vector<unsigned> ru_ul_port_id = {0};
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

/// Expert configuration.
struct ru_ofh_unit_expert_execution_config {
  /// RU timing thread.
  os_sched_affinity_bitmask ru_timing_cpu;
  /// CPU affinities per RU txrx thread.
  std::vector<os_sched_affinity_bitmask> txrx_affinities;
  /// CPU affinities per cell.
  std::vector<ru_ofh_unit_cpu_affinities_cell_config> cell_affinities = {{}};
};

/// HAL configuration.
struct ru_ofh_unit_hal_config {
  /// EAL configuration arguments.
  std::string eal_args;
};

/// Metrics configuration.
struct ru_ofh_unit_metrics_config {
  /// Metrics configuration.
  app_helpers::metrics_config metrics_cfg;
  /// Flag that control RU metrics.
  bool enable_ru_metrics = false;
};

/// gNB app Open Fronthaul Radio Unit configuration.
struct ru_ofh_unit_config {
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha = 0;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta = 0;
  /// Downlink processing time in microseconds.
  unsigned dl_processing_time = 400U;
  /// Uplink request processing time in microseconds.
  unsigned ul_processing_time = 30U;
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
  /// Metrics configuration.
  ru_ofh_unit_metrics_config metrics_cfg;
};

/// gNB app Open Fronthaul Radio Unit configuration.
struct ru_ofh_unit_parsed_config {
  /// Open Fronthaul Radio Unit configuration.
  ru_ofh_unit_config config;
  /// Base cell configuration for the Radio Unit.
  ru_ofh_unit_base_cell_config base_cell_cfg;
};

} // namespace srsran
