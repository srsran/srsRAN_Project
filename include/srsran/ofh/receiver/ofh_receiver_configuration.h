/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
};

} // namespace ofh
} // namespace srsran
