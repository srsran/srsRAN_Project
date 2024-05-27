/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

/// \brief QoS parameters for a GBR QoS flow or GBR bearer for downlink and uplink. See TS 38.473, clause 9.3.1.46.
struct gbr_qos_info_t {
  /// Maximum Bit Rate in DL.
  uint64_t max_flow_dl_bitrate;
  /// Maximum Bit Rate in UL.
  uint64_t max_flow_ul_bitrate;
  /// Guaranteed Bit Rate (provided there is data to deliver) in DL.
  uint64_t guaranteed_flow_dl_bitrate;
  /// Guaranteed Bit Rate (provided there is data to deliver) in UL.
  uint64_t guaranteed_flow_ul_bitrate;
  /// Indicates the maximum rate for lost packets that can be tolerated in the DL. Expressed in ratio of lost packets
  /// per number of packets sent, expressed in tenth of percent.Values {0,...,1000}.
  std::optional<uint16_t> max_packet_loss_rate_dl;
  /// Indicates the maximum rate for lost packets that can be tolerated in the UL. Expressed in ratio of lost packets
  /// per number of packets sent, expressed in tenth of percent.Values {0,...,1000}.
  std::optional<uint16_t> max_packet_loss_rate_ul;
};

} // namespace srsran
