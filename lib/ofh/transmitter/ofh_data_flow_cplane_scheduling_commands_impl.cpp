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

#include "ofh_data_flow_cplane_scheduling_commands_impl.h"
#include "scoped_frame_buffer.h"

using namespace srsran;
using namespace ofh;

/// Generates and returns the downlink Open Fronthaul control parameters for the given context.
static cplane_section_type1_parameters generate_ofh_control_parameters(slot_point                   slot,
                                                                       unsigned                     nof_prb,
                                                                       unsigned                     nof_symbols,
                                                                       const ru_compression_params& comp,
                                                                       data_direction               direction,
                                                                       filter_index_type            filter_type)
{
  cplane_section_type1_parameters msg_params;

  msg_params.comp_params                  = comp;
  cplane_radio_application_header& params = msg_params.radio_hdr;
  params.slot                             = slot;
  params.direction                        = direction;
  params.filter_index                     = filter_type;
  // Always start in the first symbol.
  params.start_symbol = 0U;

  cplane_common_section_0_1_3_5_fields& section = msg_params.section_fields.common_fields;
  section.section_id                            = 0;
  // Use all the resource elements.
  section.re_mask = 0xfff;
  // Use all the symbols.
  section.nof_symbols = nof_symbols;
  section.prb_start   = 0U;
  section.nof_prb     = nof_prb;

  return msg_params;
}

/// Generates and returns the eCPRI Real-Time control parameters.
static ecpri::realtime_control_parameters generate_ecpri_control_parameters(uint16_t seq_id, uint16_t eaxc)
{
  ecpri::realtime_control_parameters params;
  // Only supporting 1 Port, 1 band and 1 CC.
  params.rtc_id = eaxc;
  params.seq_id = 0;
  // Set seq_id.
  params.seq_id |= (seq_id & 0x00ff) << 8;
  // Set the E bit to 1 and subsequence ID to 0. E bit set to 1 indicates that there is no radio transport layer
  // fragmentation.
  params.seq_id |= uint16_t(1U) << 7;

  return params;
}

data_flow_cplane_scheduling_commands_impl::data_flow_cplane_scheduling_commands_impl(
    data_flow_cplane_scheduling_commands_impl_config&& config) :
  nof_symbols(config.nof_symbols),
  ru_nof_prbs(config.ru_nof_prbs),
  dl_compr_params(config.dl_compr_params),
  ul_compr_params(config.ul_compr_params),
  vlan_params(config.vlan_params),
  logger(*config.logger),
  ul_cplane_context_repo_ptr(config.ul_cplane_context_repo),
  frame_pool_ptr(config.frame_pool),
  ul_cplane_context_repo(*ul_cplane_context_repo_ptr),
  frame_pool(*frame_pool_ptr),
  eth_builder(std::move(config.eth_builder)),
  ecpri_builder(std::move(config.ecpri_builder)),
  cp_builder(std::move(config.cp_builder))
{
  srsran_assert(eth_builder, "Invalid Ethernet VLAN packet builder");
  srsran_assert(ecpri_builder, "Invalid eCPRI packet builder");
  srsran_assert(cp_builder, "Invalid Control-Plane message builder");
}

void data_flow_cplane_scheduling_commands_impl::enqueue_section_type_1_message(slot_point        slot,
                                                                               unsigned          eaxc,
                                                                               data_direction    direction,
                                                                               filter_index_type filter_type)
{
  logger.debug("Creating Control-Plane message for {} at slot={}",
               (direction == data_direction::downlink) ? "downlink" : "uplink",
               slot);

  // Get an ethernet frame buffer.
  scoped_frame_buffer  scoped_buffer(frame_pool, slot, 0U, message_type::control_plane);
  ether::frame_buffer& frame_buffer = scoped_buffer.get_next_frame();
  span<uint8_t>        buffer       = frame_buffer.data();

  // Build the Open Fronthaul control message. Only one port supported.
  units::bytes  ether_hdr_size = eth_builder->get_header_size();
  units::bytes  ecpri_hdr_size = ecpri_builder->get_header_size(ecpri::message_type::rt_control_data);
  units::bytes  offset         = ether_hdr_size + ecpri_hdr_size;
  span<uint8_t> ofh_buffer     = span<uint8_t>(buffer).last(buffer.size() - offset.value());
  const auto&   ofh_ctrl_params =
      generate_ofh_control_parameters(slot,
                                      ru_nof_prbs,
                                      nof_symbols,
                                      (direction == data_direction::downlink) ? dl_compr_params : ul_compr_params,
                                      direction,
                                      filter_type);
  unsigned bytes_written = cp_builder->build_dl_ul_radio_channel_message(ofh_buffer, ofh_ctrl_params);

  // Register the Control-Plane parameters for uplink messages.
  if (direction == data_direction::uplink) {
    ul_cplane_context_repo.add(slot, ofh_ctrl_params);
  }

  // Add eCPRI header.
  span<uint8_t> ecpri_buffer =
      span<uint8_t>(buffer).subspan(ether_hdr_size.value(), ecpri_hdr_size.value() + bytes_written);
  unsigned seq_id =
      (direction == data_direction::downlink) ? cp_dl_seq_gen.generate(eaxc) : cp_ul_seq_gen.generate(eaxc);
  ecpri_builder->build_control_packet(ecpri_buffer, generate_ecpri_control_parameters(seq_id, eaxc));

  // Update the number of bytes written.
  bytes_written += ecpri_hdr_size.value();

  // Add Ethernet header.
  span<uint8_t> eth_buffer = span<uint8_t>(buffer).first(ether_hdr_size.value() + bytes_written);
  eth_builder->build_vlan_frame(eth_buffer, vlan_params);

  frame_buffer.set_size(eth_buffer.size());
}
