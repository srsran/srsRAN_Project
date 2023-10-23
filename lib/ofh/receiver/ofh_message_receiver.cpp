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

#include "ofh_message_receiver.h"
#include "ofh_rx_window_checker.h"

using namespace srsran;
using namespace ofh;

message_receiver::message_receiver(const message_receiver_config&  config,
                                   message_receiver_dependencies&& dependencies) :
  logger(*dependencies.logger),
  vlan_params(config.vlan_params),
  ul_prach_eaxc(config.prach_eaxc),
  ul_eaxc(config.ul_eaxc),
  window_checker(*dependencies.window_checker),
  vlan_decoder(std::move(dependencies.eth_frame_decoder)),
  ecpri_decoder(std::move(dependencies.ecpri_decoder)),
  uplane_decoder(std::move(dependencies.uplane_decoder)),
  data_flow_uplink(std::move(dependencies.data_flow_uplink)),
  data_flow_prach(std::move(dependencies.data_flow_prach))
{
  srsran_assert(vlan_decoder, "Invalid VLAN decoder");
  srsran_assert(ecpri_decoder, "Invalid eCPRI decoder");
  srsran_assert(uplane_decoder, "Invalid User-Plane decoder");
  srsran_assert(data_flow_uplink, "Invalid uplink IQ data flow");
  srsran_assert(data_flow_prach, "Invalid uplink PRACH IQ data flow");
}

void message_receiver::on_new_frame(span<const uint8_t> payload)
{
  ether::vlan_frame_params eth_params;
  span<const uint8_t>      ecpri_pdu = vlan_decoder->decode(payload, eth_params);
  if (ecpri_pdu.empty() || should_ethernet_frame_be_filtered(eth_params)) {
    return;
  }

  ecpri::packet_parameters ecpri_params;
  span<const uint8_t>      ofh_pdu = ecpri_decoder->decode(ecpri_pdu, ecpri_params);
  if (ofh_pdu.empty() || should_ecpri_packet_be_filtered(ecpri_params)) {
    return;
  }

  // Verify the sequence identifier.
  const ecpri::iq_data_parameters& ecpri_iq_params = variant_get<ecpri::iq_data_parameters>(ecpri_params.type_params);
  int                              nof_skipped_seq_id =
      seq_id_checker.update_and_compare_seq_id(ecpri_iq_params.pc_id, (ecpri_iq_params.seq_id >> 8));
  // Drop the message when it is from the past.
  if (nof_skipped_seq_id < 0) {
    logger.debug("Dropping Open Fronthaul User-Plane message as sequence identifier was from the past");

    return;
  } else if (nof_skipped_seq_id > 0) {
    logger.warning("Detected {} lost messages", nof_skipped_seq_id);
  }

  slot_symbol_point slot_point = uplane_decoder->peek_slot_symbol_point(ofh_pdu);
  if (!slot_point.get_slot().valid()) {
    return;
  }

  // Fill the reception window statistics.
  window_checker.update_rx_window_statistics(slot_point);

  unsigned eaxc = variant_get<ecpri::iq_data_parameters>(ecpri_params.type_params).pc_id;
  if (is_a_prach_message(uplane_decoder->peek_filter_index(ofh_pdu))) {
    data_flow_prach->decode_type1_message(eaxc, ofh_pdu);

    return;
  }

  data_flow_uplink->decode_type1_message(eaxc, ofh_pdu);
}

bool message_receiver::should_ecpri_packet_be_filtered(const ecpri::packet_parameters& ecpri_params) const
{
  if (ecpri_params.header.msg_type != ecpri::message_type::iq_data) {
    logger.debug("Dropping Open Fronthaul User-Plane message as decoded eCPRI message type is not IQ data");

    return true;
  }

  const ecpri::iq_data_parameters& ecpri_iq_params = variant_get<ecpri::iq_data_parameters>(ecpri_params.type_params);
  if ((std::find(ul_eaxc.begin(), ul_eaxc.end(), ecpri_iq_params.pc_id) == ul_eaxc.end()) &&
      (std::find(ul_prach_eaxc.begin(), ul_prach_eaxc.end(), ecpri_iq_params.pc_id) == ul_prach_eaxc.end())) {
    logger.debug("Dropping Open Fronthaul User-Plane message as decoded eAxC is {}", ecpri_iq_params.pc_id);

    return true;
  }

  return false;
}

bool message_receiver::should_ethernet_frame_be_filtered(const ether::vlan_frame_params& eth_params) const
{
  if (eth_params.mac_src_address != vlan_params.mac_src_address) {
    logger.debug("Dropping Ethernet packet as source MAC addresses doesn't match(detected={:x}, "
                 "expected={:x})",
                 span<const uint8_t>(eth_params.mac_src_address),
                 span<const uint8_t>(vlan_params.mac_src_address));

    return true;
  }

  if (eth_params.mac_dst_address != vlan_params.mac_dst_address) {
    logger.debug("Dropping Ethernet packet as destination MAC addresses doesn't match(detected={:x}, "
                 "expected={:x})",
                 span<const uint8_t>(eth_params.mac_dst_address),
                 span<const uint8_t>(vlan_params.mac_dst_address));

    return true;
  }

  if (eth_params.eth_type != vlan_params.eth_type) {
    logger.debug("Dropping Ethernet packet as decoded Ethernet type is {} and it is expected {}",
                 eth_params.eth_type,
                 vlan_params.eth_type);

    return true;
  }

  return false;
}
