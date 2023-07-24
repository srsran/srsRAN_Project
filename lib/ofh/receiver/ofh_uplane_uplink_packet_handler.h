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
  uplane_uplink_packet_handler_config(srslog::basic_logger& logger_, uplink_cplane_context_repository& cplane_repo_) :
    logger(logger_), cplane_repo(cplane_repo_)
  {
  }

  /// Logger.
  srslog::basic_logger& logger;
  /// PRACH Control-Plane enabled flag.
  bool is_prach_cp_enabled;
  /// Uplink Control-Plane context repository.
  uplink_cplane_context_repository& cplane_repo;
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
};

/// Open Fronthaul uplink packet handler.
class uplane_uplink_packet_handler
{
public:
  explicit uplane_uplink_packet_handler(uplane_uplink_packet_handler_config&& config);

  // Decodes the given packet and returns the results.
  expected<message_decoder_results> decode_packet(span<const uint8_t> packet);

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
  uplink_cplane_context_repository&                     cplane_repo;
  ether::vlan_frame_params                              vlan_params;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_prach_eaxc;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  std::unique_ptr<uplane_message_decoder>               uplane_decoder;
  std::unique_ptr<ecpri::packet_decoder>                ecpri_decoder;
  std::unique_ptr<ether::vlan_frame_decoder>            eth_frame_decoder;
};

} // namespace ofh
} // namespace srsran
