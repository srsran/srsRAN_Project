/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ofh/ofh_cplane_message_builder.h"
#include "srsran/ofh/ofh_uplane_message_builder.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul transmitter configuration.
struct transmitter_config {
  /// Channel bandwidth.
  bs_channel_bandwidth_fr1 bw;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Uplink PRACH eAxC.
  optional<unsigned> ul_prach_eaxc;
  /// Downlink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// Destination MAC address.
  ether::mac_address mac_dst_address;
  /// Source MAC address.
  ether::mac_address mac_src_address;
  /// Tag control information field.
  uint16_t tci;
  /// Ethernet interface name;
  std::string interface;
  /// RU working bandwidth.
  bs_channel_bandwidth_fr1 ru_working_bw;
  /// Open Fronthaul symbol handler configuration.
  symbol_handler_config symbol_handler_cfg;
  /// Downlink compression parameters.
  ru_compression_params dl_compr_params;
  /// Uplink compression parameters.
  ru_compression_params ul_compr_params;
  /// \brief Downlink broadcast flag.
  ///
  /// If this flag is enabled the same downlink data will be send to all the configured downlink eAxCs.
  bool downlink_broadcast;
  /// IQ samples scaling factor.
  float iq_scaling;
};

} // namespace ofh
} // namespace srsran
