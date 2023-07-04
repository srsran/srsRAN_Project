/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_uplink_handler.h"

using namespace srsran;
using namespace ofh;

uplink_handler::uplink_handler(const uplink_handler_config& config, uplink_handler_depen&& depen) :
  logger(*depen.logger),
  vlan_params(config.vlan_params),
  ul_prach_eaxc(config.ul_prach_eaxc),
  ul_eaxc(config.ul_eaxc),
  vlan_decoder(std::move(depen.eth_frame_decoder)),
  ecpri_decoder(std::move(depen.ecpri_decoder)),
  data_flow_uplink(std::move(depen.data_flow_uplink)),
  data_flow_prach(std::move(depen.data_flow_prach))
{
  srsran_assert(vlan_decoder, "Invalid VLAN decoder");
  srsran_assert(ecpri_decoder, "Invalid eCPRI decoder");
  srsran_assert(data_flow_uplink, "Invalid uplink IQ data flow");
  srsran_assert(data_flow_prach, "Invalid uplink PRACH IQ data flow");
}

void uplink_handler::on_new_frame(span<const uint8_t> payload)
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

  unsigned eaxc = variant_get<ecpri::iq_data_parameters>(ecpri_params.type_params).pc_id;
  if (is_a_prach_message(peek_message_filter_type(ofh_pdu))) {
    data_flow_prach->decode_type1_message(eaxc, ofh_pdu);

    return;
  }

  data_flow_uplink->decode_type1_message(eaxc, ofh_pdu);
}

filter_index_type uplink_handler::peek_message_filter_type(span<const uint8_t> msg) const
{
  // Filter index is codified in the first byte, the 4 LSB.
  return to_filter_index_type((msg[0] & 0xf));
}

bool uplink_handler::should_ecpri_packet_be_filtered(const ecpri::packet_parameters& ecpri_params) const
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

bool uplink_handler::should_ethernet_frame_be_filtered(const ether::vlan_frame_params& eth_params) const
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
