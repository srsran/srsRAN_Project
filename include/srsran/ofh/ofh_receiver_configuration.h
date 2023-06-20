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
#include "srsran/ofh/ofh_uplane_message_decoder.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver configuration.
struct receiver_config {
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// DU uplink bandwidth in PRBs.
  unsigned du_ul_slot_nof_prbs;
  /// RU bandwidth in PRBs.
  unsigned ru_nof_prbs;
  /// PRACH Contol-Plane enabled flag.
  bool is_prach_cp_enabled;
  /// Uplink PRACH eAxC.
  unsigned ul_prach_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Destination MAC address.
  ether::mac_address mac_dst_address;
  /// Source MAC address.
  ether::mac_address mac_src_address;
  /// Tag control information field.
  uint16_t tci;
};

} // namespace ofh
} // namespace srsran
