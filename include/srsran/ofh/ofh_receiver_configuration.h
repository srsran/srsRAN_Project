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
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {
namespace ofh {

/// \brief Structure storing the reception window timing parameters.
struct du_rx_window_timing_parameters {
  /// Offset from the current OTA symbol to the end of UL User-Plane reception window.
  std::chrono::microseconds Ta4_max;
  /// Offset from the current OTA symbol to the start of UL User-Plane reception window.
  std::chrono::microseconds Ta4_min;
};

/// Open Fronthaul receiver configuration.
struct receiver_config {
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// RU bandwidth in PRBs.
  unsigned ru_nof_prbs;
  /// PRACH Contol-Plane enabled flag.
  bool is_prach_cp_enabled;
  /// PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// Uplink static compression header flag.
  bool is_uplink_static_compr_hdr_enabled;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Destination MAC address.
  ether::mac_address mac_dst_address;
  /// Source MAC address.
  ether::mac_address mac_src_address;
  /// Tag control information field.
  uint16_t tci;
  /// Reception window timing parameters.
  du_rx_window_timing_parameters rx_timing_params;
};

} // namespace ofh
} // namespace srsran
