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

#include "ofh_uplane_uplink_packet_handler.h"

using namespace srsran;
using namespace ofh;

uplane_uplink_packet_handler::uplane_uplink_packet_handler(uplane_uplink_packet_handler_config&& config) :
  logger(config.logger),
  is_prach_cp_enabled(config.is_prach_cp_enabled),
  cplane_repo(config.cplane_repo),
  vlan_params(config.vlan_params),
  ul_prach_eaxc(config.ul_prach_eaxc),
  ul_eaxc(config.ul_eaxc),
  uplane_decoder(std::move(config.uplane_decoder)),
  ecpri_decoder(std::move(config.ecpri_decoder)),
  eth_frame_decoder(std::move(config.eth_frame_decoder))
{
  srsran_assert(uplane_decoder, "Invalid User-Plane packet decoder");
  srsran_assert(ecpri_decoder, "Invalid eCPRI packet decoder");
  srsran_assert(eth_frame_decoder, "Invalid Ethernet frame decoder");
}

expected<message_decoder_results> uplane_uplink_packet_handler::decode_packet(span<const uint8_t> payload)
{
  ether::vlan_frame_params eth_params;
  span<const uint8_t>      ecpri_pdu = eth_frame_decoder->decode(payload, eth_params);
  if (ecpri_pdu.empty() || should_ethernet_frame_be_filtered(eth_params)) {
    return {default_error_t({})};
  }

  ecpri::packet_parameters ecpri_params;
  span<const uint8_t>      ofh_pdu = ecpri_decoder->decode(ecpri_pdu, ecpri_params);
  if (ofh_pdu.empty() || should_ecpri_packet_be_filtered(ecpri_params)) {
    return {default_error_t({})};
  }

  message_decoder_results results;
  results.eaxc = variant_get<ecpri::iq_data_parameters>(ecpri_params.type_params).pc_id;
  uplane_message_decoder_results& uplane_results = results.uplane_results;
  if (!uplane_decoder->decode(uplane_results, ofh_pdu)) {
    return {default_error_t({})};
  }

  if (should_uplane_packet_be_filtered(results)) {
    return {default_error_t({})};
  }

  return {std::move(results)};
}

bool uplane_uplink_packet_handler::should_ethernet_frame_be_filtered(const ether::vlan_frame_params& eth_params) const
{
  if (eth_params.mac_src_address != vlan_params.mac_src_address) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as source MAC addresses doesn't match(detected={:x}, "
                 "expected={:x})",
                 span<const uint8_t>(eth_params.mac_src_address),
                 span<const uint8_t>(vlan_params.mac_src_address));

    return true;
  }

  if (eth_params.mac_dst_address != vlan_params.mac_dst_address) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as destination MAC addresses doesn't match(detected={:x}, "
                 "expected={:x})",
                 span<const uint8_t>(eth_params.mac_dst_address),
                 span<const uint8_t>(vlan_params.mac_dst_address));

    return true;
  }

  if (eth_params.eth_type != vlan_params.eth_type) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as decoded Ethernet type is {} and it is expected {}",
                 eth_params.eth_type,
                 vlan_params.eth_type);

    return true;
  }

  return false;
}

bool uplane_uplink_packet_handler::should_ecpri_packet_be_filtered(const ecpri::packet_parameters& ecpri_params) const
{
  if (ecpri_params.header.msg_type != ecpri::message_type::iq_data) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as decoded eCPRI message type is not IQ data");

    return true;
  }

  const ecpri::iq_data_parameters& ecpri_iq_params = variant_get<ecpri::iq_data_parameters>(ecpri_params.type_params);
  if ((std::find(ul_eaxc.begin(), ul_eaxc.end(), ecpri_iq_params.pc_id) == ul_eaxc.end()) &&
      (std::find(ul_prach_eaxc.begin(), ul_prach_eaxc.end(), ecpri_iq_params.pc_id) == ul_prach_eaxc.end())) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as decoded eAxC is {}", ecpri_iq_params.pc_id);

    return true;
  }

  // :TODO: check sequence id.

  return false;
}

bool uplane_uplink_packet_handler::should_uplane_packet_be_filtered(const message_decoder_results& results) const
{
  const uplane_message_decoder_results& uplane_results = results.uplane_results;
  if (uplane_results.params.filter_index == filter_index_type::reserved) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as decoded filter index={} for slot={}, symbol={}",
                 to_value(uplane_results.params.filter_index),
                 uplane_results.params.slot,
                 uplane_results.params.symbol_id);

    return true;
  }

  // When Control-Plane message for PRACH is not configured, skip the check.
  if (!is_prach_cp_enabled && is_a_prach_message(uplane_results.params.filter_index)) {
    return false;
  }

  const uplane_message_params& params = uplane_results.params;
  expected<ul_cplane_context>  ex_cp_context =
      cplane_repo.get(params.slot, params.symbol_id, params.filter_index, results.eaxc);

  if (!ex_cp_context) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as no Control-Packet associated was found for slot={}, "
                 "symbol={}, eAxC={}",
                 params.slot,
                 params.symbol_id,
                 results.eaxc);

    return true;
  }

  // Check the PRBs.
  const ul_cplane_context& cp_context = ex_cp_context.value();
  for (const auto& up_section : uplane_results.sections) {
    if (up_section.start_prb > MAX_NOF_PRBS - 1) {
      logger.debug("Dropping Open Fronthaul User-Plane packet as the first PRB index {} is not valid",
                   up_section.start_prb);

      return true;
    }

    if (up_section.start_prb + up_section.nof_prbs > MAX_NOF_PRBS) {
      logger.debug("Dropping Open Fronthaul User-Plane packet as the last PRB index {} is not valid",
                   up_section.start_prb + up_section.nof_prbs);

      return true;
    }

    if (!up_section.is_every_rb_used) {
      logger.debug(
          "Dropping Open Fronthaul User-Plane packet as 'every other resource block is used' mode is not supported");

      return true;
    }

    if (!up_section.use_current_symbol_number) {
      logger.debug("Dropping Open Fronthaul User-Plane packet as 'increment the current symbol number and use that' "
                   "mode is not supported");

      return true;
    }

    if (up_section.start_prb < cp_context.prb_start ||
        (up_section.start_prb + up_section.nof_prbs) > (cp_context.prb_start + cp_context.nof_prb)) {
      logger.debug("Dropping Open Fronthaul User-Plane packet as PRB indexes {}:{} don't match Control-Packet {}:{}.",
                   up_section.start_prb,
                   up_section.nof_prbs,
                   cp_context.prb_start,
                   cp_context.nof_prb);
      return true;
    }
  }

  return false;
}
