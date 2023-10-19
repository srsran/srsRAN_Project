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

#include "srsran/ofh/compression/iq_decompressor.h"
#include "srsran/ofh/ecpri/ecpri_packet_decoder.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/receiver/ofh_receiver_timing_parameters.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver configuration.
struct receiver_config {
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// Destination MAC address.
  ether::mac_address mac_dst_address;
  /// Source MAC address.
  ether::mac_address mac_src_address;
  /// Tag control information field.
  uint16_t tci;
  /// Reception window timing parameters.
  du_rx_window_timing_parameters rx_timing_params;
  /// \brief RU operating bandwidth.
  ///
  /// Set this option when the operating bandwidth of the RU is larger than the configured bandwidth of the cell.
  bs_channel_bandwidth_fr1 ru_operating_bw;
  /// Uplink compression parameters.
  ofh::ru_compression_params ul_compression_params;
  /// PRACH compression parameters.
  ofh::ru_compression_params prach_compression_params;
  /// Uplink static compression header flag.
  bool is_uplink_static_compr_hdr_enabled = true;
  /// Enables the Control-Plane PRACH message signalling.
  bool is_prach_control_plane_enabled = false;
  /// If set to true, the payload size encoded in a eCPRI header is ignored.
  bool ignore_ecpri_payload_size_field = false;
};

} // namespace ofh
} // namespace srsran
