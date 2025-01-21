/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_message_receiver.h"
#include "ofh_rx_window_checker.h"
#include "srsran/instrumentation/traces/ofh_traces.h"

using namespace srsran;
using namespace ofh;

message_receiver_impl::message_receiver_impl(const message_receiver_config&  config,
                                             message_receiver_dependencies&& dependencies) :
  logger(*dependencies.logger),
  sector_id(config.sector),
  nof_symbols(config.nof_symbols),
  scs(config.scs),
  vlan_params(config.vlan_params),
  ul_prach_eaxc(config.prach_eaxc),
  ul_eaxc(config.ul_eaxc),
  window_checker(*dependencies.window_checker),
  seq_id_checker(std::move(dependencies.seq_id_checker)),
  vlan_decoder(std::move(dependencies.eth_frame_decoder)),
  ecpri_decoder(std::move(dependencies.ecpri_decoder)),
  data_flow_uplink(std::move(dependencies.data_flow_uplink)),
  data_flow_prach(std::move(dependencies.data_flow_prach)),
  eth_receiver(std::move(dependencies.eth_receiver))
{
  srsran_assert(vlan_decoder, "Invalid VLAN decoder");
  srsran_assert(ecpri_decoder, "Invalid eCPRI decoder");
  srsran_assert(data_flow_uplink, "Invalid uplink IQ data flow");
  srsran_assert(data_flow_prach, "Invalid uplink PRACH IQ data flow");
  srsran_assert(seq_id_checker, "Invalid sequence id checker");
  srsran_assert(eth_receiver, "Invalid Ethernet receiver");
}

void message_receiver_impl::on_new_frame(ether::unique_rx_buffer buffer)
{
  process_new_frame(std::move(buffer));
}

void message_receiver_impl::process_new_frame(ether::unique_rx_buffer buffer)
{
  span<const uint8_t> payload = buffer.data();

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
  const ecpri::iq_data_parameters& ecpri_iq_params = std::get<ecpri::iq_data_parameters>(ecpri_params.type_params);
  unsigned                         eaxc            = ecpri_iq_params.pc_id;
  int nof_skipped_seq_id = seq_id_checker->update_and_compare_seq_id(eaxc, (ecpri_iq_params.seq_id >> 8));
  // Drop the message when it is from the past.
  if (nof_skipped_seq_id < 0) {
    logger.info("Sector#{}: dropped received Open Fronthaul User-Plane packet for eAxC value '{}' as sequence "
                "identifier field is "
                "from the past",
                sector_id,
                eaxc);
    return;
  }
  if (nof_skipped_seq_id > 0) {
    logger.warning("Sector#{}: potentially lost '{}' messages sent by the RU", sector_id, nof_skipped_seq_id);
  }

  expected<slot_symbol_point, std::string> slot_point =
      uplane_peeker::peek_slot_symbol_point(ofh_pdu, nof_symbols, scs);
  if (!slot_point.has_value()) {
    logger.info(
        "Sector#{}: dropped received Open Fronthaul User-Plane packet as the slot could not be peeked with error: {}",
        sector_id,
        slot_point.error());

    return;
  }

  // Fill the reception window statistics.
  window_checker.update_rx_window_statistics(slot_point.value());

  // Peek the filter index and check that it is valid.
  expected<filter_index_type, const char*> filter_type = uplane_peeker::peek_filter_index(ofh_pdu);
  if (!filter_type.has_value()) {
    logger.info("Sector#{}: dropped received Open Fronthaul User-Plane message as the filter index could not be peeked "
                "with error: {}",
                sector_id,
                filter_type.error());

    return;
  }

  trace_point decode_tp = ofh_tracer.now();
  if (is_a_prach_message(filter_type.value())) {
    data_flow_prach->decode_type1_message(eaxc, ofh_pdu);
    ofh_tracer << trace_event("ofh_receiver_decode_prach", decode_tp);
    return;
  }

  data_flow_uplink->decode_type1_message(eaxc, ofh_pdu);
  ofh_tracer << trace_event("ofh_receiver_decode_data", decode_tp);
}

bool message_receiver_impl::should_ecpri_packet_be_filtered(const ecpri::packet_parameters& ecpri_params) const
{
  if (ecpri_params.header.msg_type != ecpri::message_type::iq_data) {
    logger.info(
        "Sector#{}: dropped received Open Fronthaul User-Plane packet as decoded eCPRI message type is not for IQ data",
        sector_id);

    return true;
  }

  const ecpri::iq_data_parameters& ecpri_iq_params = std::get<ecpri::iq_data_parameters>(ecpri_params.type_params);
  if ((std::find(ul_eaxc.begin(), ul_eaxc.end(), ecpri_iq_params.pc_id) == ul_eaxc.end()) &&
      (std::find(ul_prach_eaxc.begin(), ul_prach_eaxc.end(), ecpri_iq_params.pc_id) == ul_prach_eaxc.end())) {
    logger.info("Sector#{}: dropped received Open Fronthaul User-Plane packet as decoded eAxC value '{}' is not "
                "configured in reception",
                sector_id,
                ecpri_iq_params.pc_id);

    return true;
  }

  return false;
}

bool message_receiver_impl::should_ethernet_frame_be_filtered(const ether::vlan_frame_params& eth_params) const
{
  if (eth_params.mac_src_address != vlan_params.mac_src_address) {
    logger.debug("Sector#{}: dropped received Ethernet frame as source MAC addresses do not match (detected={:02X}, "
                 "expected={:02X})",
                 sector_id,
                 span<const uint8_t>(eth_params.mac_src_address),
                 span<const uint8_t>(vlan_params.mac_src_address));

    return true;
  }

  if (eth_params.mac_dst_address != vlan_params.mac_dst_address) {
    logger.debug(
        "Sector#{}: dropped received Ethernet frame as destination MAC addresses do not match match (detected={:02X}, "
        "expected={:02X})",
        sector_id,
        span<const uint8_t>(eth_params.mac_dst_address),
        span<const uint8_t>(vlan_params.mac_dst_address));

    return true;
  }

  if (eth_params.eth_type != vlan_params.eth_type) {
    logger.info("Sector#{}: dropped received Ethernet frame as decoded Ethernet type is '{}' but expected '{}'",
                sector_id,
                eth_params.eth_type,
                vlan_params.eth_type);

    return true;
  }

  return false;
}
