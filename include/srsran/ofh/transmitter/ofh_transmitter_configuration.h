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
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/compression/iq_compressor.h"
#include "srsran/ofh/ecpri/ecpri_packet_builder.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_builder.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/serdes/ofh_cplane_message_builder.h"
#include "srsran/ofh/serdes/ofh_uplane_message_builder.h"
#include "srsran/ofh/transmitter/ofh_transmitter_timing_parameters.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul transmitter configuration.
struct transmitter_config {
  /// Radio sector identifier.
  unsigned sector;
  /// Channel bandwidth.
  bs_channel_bandwidth_fr1 bw;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Transmission window timing parameters.
  tx_window_timing_parameters tx_timing_params;
  /// Downlink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// PRACH Contol-Plane enabled flag.
  bool is_prach_cp_enabled;
  /// Destination MAC address.
  ether::mac_address mac_dst_address;
  /// Source MAC address.
  ether::mac_address mac_src_address;
  /// Tag control information field.
  uint16_t tci;
  /// Ethernet interface name or identifier.
  std::string interface;
  /// Promiscuous mode flag.
  bool is_promiscuous_mode_enabled;
  /// MTU size.
  units::bytes mtu_size;
  /// RU working bandwidth.
  bs_channel_bandwidth_fr1 ru_working_bw;
  /// Downlink compression parameters.
  ru_compression_params dl_compr_params;
  /// Uplink compression parameters.
  ru_compression_params ul_compr_params;
  /// PRACH compression parameters.
  ru_compression_params prach_compr_params;
  /// Downlink static compression header flag.
  bool is_downlink_static_compr_hdr_enabled;
  /// \brief Downlink broadcast flag.
  ///
  /// If this flag is enabled the same downlink data will be send to all the configured downlink eAxCs.
  bool downlink_broadcast;
  /// IQ samples scaling factor.
  float iq_scaling;
  /// Downlink processing time in microseconds.
  std::chrono::microseconds dl_processing_time;
  /// Optional TDD configuration.
  optional<tdd_ul_dl_config_common> tdd_config;
  /// Indicates if DPDK should be used by the underlying implementation.
  bool uses_dpdk;
};

} // namespace ofh
} // namespace srsran
