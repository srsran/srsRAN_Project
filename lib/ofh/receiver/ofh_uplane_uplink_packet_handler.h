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

#include "../support/uplink_cplane_context_repository.h"
#include "srsran/adt/expected.h"
#include "srsran/ofh/ecpri/ecpri_packet_decoder.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/serdes/ofh_message_decoder_properties.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"

namespace srsran {
namespace ofh {

/// User-Plane uplink packet handler configuration.
struct uplane_uplink_packet_handler_config {
  explicit uplane_uplink_packet_handler_config(srslog::basic_logger& logger_) : logger(logger_) {}

  /// Logger.
  srslog::basic_logger& logger;
  /// PRACH Control-Plane enabled flag.
  bool is_prach_cp_enabled;
  /// VLAN ethernet frame parameters.
  ether::vlan_frame_params vlan_params;
  /// Uplink PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_prach_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// User-Plane message decoder.
  std::unique_ptr<uplane_message_decoder> uplane_decoder;
  /// eCPRI packet decoder.
  std::unique_ptr<ecpri::packet_decoder> ecpri_decoder;
  /// Ethernet frame decoder.
  std::unique_ptr<ether::vlan_frame_decoder> eth_frame_decoder;
  /// Uplink Control-Plane context repository.
  std::shared_ptr<uplink_cplane_context_repository> cplane_repo;
};

/// Ethernet and eCPRI decoding results.
struct eth_and_ecpri_decoding_results {
  /// Decoded eAxC.
  unsigned eaxc;
  /// Open Fronthaul packet.
  span<const uint8_t> ofh_packet;
};

/// Open Fronthaul uplink packet handler.
class uplane_uplink_packet_handler
{
public:
  explicit uplane_uplink_packet_handler(uplane_uplink_packet_handler_config&& config);

  /// Decodes the given packet and returns the results.
  expected<eth_and_ecpri_decoding_results> decode_eth_and_ecpri_packet(span<const uint8_t> packet);

  /// Decodes the given packet and returns the results.
  expected<message_decoder_results> decode_ofh_packet(unsigned eaxc, span<const uint8_t> packet);

  /// Peeks and returns the slot symbol point of the given packet.
  slot_symbol_point peek_slot_symbol_point(span<const uint8_t> packet) const;

private:
  /// Returns true if the ethernet frame represented by the given eth parameters should be filtered, otherwise false.
  bool should_ethernet_frame_be_filtered(const ether::vlan_frame_params& eth_params) const;

  /// Returns true if the eCPRI packet represented by the given eCPRI parameters should be filtered, otherwise false.
  bool should_ecpri_packet_be_filtered(const ecpri::packet_parameters& ecpri_params) const;

  /// Returns true if the User-Plane packet represented by the given User-Plane results should be filtered, otherwise
  /// false.
  bool should_uplane_packet_be_filtered(const message_decoder_results& results) const;

private:
  srslog::basic_logger&                                 logger;
  const bool                                            is_prach_cp_enabled;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_prach_eaxc;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  ether::vlan_frame_params                              vlan_params;
  std::unique_ptr<uplane_message_decoder>               uplane_decoder;
  std::unique_ptr<ecpri::packet_decoder>                ecpri_decoder;
  std::unique_ptr<ether::vlan_frame_decoder>            eth_frame_decoder;
  std::shared_ptr<uplink_cplane_context_repository>     cplane_repo_ptr;
  uplink_cplane_context_repository&                     cplane_repo;
};

} // namespace ofh
} // namespace srsran
