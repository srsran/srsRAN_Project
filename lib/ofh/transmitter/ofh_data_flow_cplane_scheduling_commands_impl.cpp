/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/ran/resource_block.h"

using namespace srsran;
using namespace ofh;

/// \brief Initializes Open Fronthaul Control-Plane radio application header parameters.
static void init_radio_app_header_parameters(cplane_radio_application_header& radio_hdr,
                                             slot_point                       slot,
                                             uint8_t                          start_symbol,
                                             data_direction                   direction,
                                             filter_index_type                filter_type)
{
  radio_hdr.slot         = slot;
  radio_hdr.direction    = direction;
  radio_hdr.filter_index = filter_type;
  radio_hdr.start_symbol = start_symbol;
}

/// \brief Initializes common fields for Open Fronthaul Control-Plane sections of types 0, 1, 3 and 5.
static void init_common_section_0_1_3_5_fields(cplane_common_section_0_1_3_5_fields& section,
                                               unsigned                              nof_prb,
                                               unsigned                              nof_symbols)
{
  section.section_id = 0;
  // Use all the resource elements.
  section.re_mask = 0xfff;
  // Use all the symbols.
  section.nof_symbols = nof_symbols;
  section.prb_start   = 0U;
  section.nof_prb     = nof_prb;
}

/// \brief Generates and returns the downlink Open Fronthaul control parameters of section type 1 for the given context.
static cplane_section_type1_parameters
generate_section1_control_parameters(const data_flow_cplane_type_1_context& context,
                                     unsigned                               nof_prb,
                                     const ru_compression_params&           comp)
{
  cplane_section_type1_parameters msg_params;

  msg_params.comp_params = comp;

  // Initialize radio application header.
  init_radio_app_header_parameters(
      msg_params.radio_hdr, context.slot, context.symbol_range.start(), context.direction, context.filter_type);
  // Initialize common fields.
  init_common_section_0_1_3_5_fields(msg_params.section_fields.common_fields, nof_prb, context.symbol_range.length());

  return msg_params;
}

/// \brief Converts PRACH subcarrier spacing numerology to ORAN specific value.
static cplane_scs cplane_convert_scs(prach_subcarrier_spacing scs)
{
  switch (scs) {
    case prach_subcarrier_spacing::kHz1_25:
      return cplane_scs::kHz1_25;
    case prach_subcarrier_spacing::kHz5:
      return cplane_scs::kHz5;
    default:
      return static_cast<cplane_scs>(to_numerology_value(scs));
  }
}

/// \brief Generates and returns the downlink Open Fronthaul control parameters of section type 3
/// for PRACH using the given context.
///
/// \note numSymbol parameter is number of PRACH repetitions in the case of PRACH.
static cplane_section_type3_parameters
generate_prach_control_parameters(const data_flow_cplane_scheduling_prach_context& context,
                                  const ru_compression_params&                     comp,
                                  unsigned                                         ru_nof_prb)
{
  cplane_section_type3_parameters msg_params;

  msg_params.comp_params = comp;
  msg_params.scs         = cplane_convert_scs(context.prach_scs);
  msg_params.time_offset = context.time_offset;
  msg_params.cpLength    = 0;
  // TODO: see if this parameter needs to be derived from the PRACH context.
  msg_params.fft_size = cplane_fft_size::fft_4096;

  // Initialize radio application header.
  init_radio_app_header_parameters(
      msg_params.radio_hdr, context.slot, context.start_symbol, data_direction::uplink, context.filter_type);

  // Initialize common fields (numSymbol parameter is number of PRACH repetitions in the case of PRACH).
  init_common_section_0_1_3_5_fields(
      msg_params.section_fields.common_fields, context.prach_nof_rb, context.nof_repetitions);

  double offset_to_prach_Hz = context.prach_start_re * ra_scs_to_Hz(context.prach_scs);
  double total_bw_Hz        = 1000 * scs_to_khz(context.scs) * ru_nof_prb * NOF_SUBCARRIERS_PER_RB;
  double freq_offset_Hz     = -(total_bw_Hz / 2 - offset_to_prach_Hz);

  // Calculate offset in number of (\f$\Delta f\f$).
  double freq_offset                         = freq_offset_Hz / (ra_scs_to_Hz(context.prach_scs) / 2);
  msg_params.section_fields.frequency_offset = static_cast<int>(freq_offset);

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
    const data_flow_cplane_scheduling_commands_impl_config&  config,
    data_flow_cplane_scheduling_commands_impl_dependencies&& dependencies) :
  logger(*dependencies.logger),
  ru_nof_prbs(config.ru_nof_prbs),
  dl_compr_params(config.dl_compr_params),
  ul_compr_params(config.ul_compr_params),
  prach_compr_params(config.prach_compr_params),
  vlan_params(config.vlan_params),
  ul_cplane_context_repo_ptr(dependencies.ul_cplane_context_repo),
  frame_pool_ptr(dependencies.frame_pool),
  ul_cplane_context_repo(*ul_cplane_context_repo_ptr),
  frame_pool(*frame_pool_ptr),
  eth_builder(std::move(dependencies.eth_builder)),
  ecpri_builder(std::move(dependencies.ecpri_builder)),
  cp_builder(std::move(dependencies.cp_builder))
{
  srsran_assert(eth_builder, "Invalid Ethernet VLAN packet builder");
  srsran_assert(ecpri_builder, "Invalid eCPRI packet builder");
  srsran_assert(cp_builder, "Invalid Control-Plane message builder");
  srsran_assert(frame_pool_ptr, "Invalid frame pool");
  srsran_assert(ul_cplane_context_repo_ptr, "Invalid UL repository");
}

void data_flow_cplane_scheduling_commands_impl::enqueue_section_type_1_message(
    const data_flow_cplane_type_1_context& context)
{
  data_direction direction = context.direction;
  slot_point     slot      = context.slot;
  logger.debug("Packing a {} type 1 Control-Plane message for slot '{}' and eAxC '{}'",
               (direction == data_direction::downlink) ? "downlink" : "uplink",
               slot,
               context.eaxc);

  // Get an ethernet frame buffer.
  scoped_frame_buffer  scoped_buffer(frame_pool, slot, 0U, message_type::control_plane, direction);
  ether::frame_buffer& frame_buffer = scoped_buffer.get_next_frame();
  span<uint8_t>        buffer       = frame_buffer.data();

  // Build the Open Fronthaul control message. Only one port supported.
  units::bytes  ether_hdr_size  = eth_builder->get_header_size();
  units::bytes  ecpri_hdr_size  = ecpri_builder->get_header_size(ecpri::message_type::rt_control_data);
  units::bytes  offset          = ether_hdr_size + ecpri_hdr_size;
  span<uint8_t> ofh_buffer      = span<uint8_t>(buffer).last(buffer.size() - offset.value());
  const auto&   ofh_ctrl_params = generate_section1_control_parameters(
      context, ru_nof_prbs, (direction == data_direction::downlink) ? dl_compr_params : ul_compr_params);
  unsigned bytes_written = cp_builder->build_dl_ul_radio_channel_message(ofh_buffer, ofh_ctrl_params);

  unsigned eaxc = context.eaxc;
  // Register the Control-Plane parameters for uplink messages.
  if (direction == data_direction::uplink) {
    ul_cplane_context_repo.add(slot,
                               eaxc,
                               {ofh_ctrl_params.radio_hdr,
                                ofh_ctrl_params.section_fields.common_fields.prb_start,
                                ofh_ctrl_params.section_fields.common_fields.nof_prb,
                                ofh_ctrl_params.section_fields.common_fields.nof_symbols});
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

void data_flow_cplane_scheduling_commands_impl::enqueue_section_type_3_prach_message(
    const data_flow_cplane_scheduling_prach_context& context)
{
  slot_point slot = context.slot;
  logger.debug("Packing a type 3 PRACH Control-Plane message for slot '{}' and eAxC '{}'", slot, context.eaxc);

  // Get an ethernet frame buffer.
  scoped_frame_buffer scoped_buffer(
      frame_pool, slot, context.start_symbol, message_type::control_plane, data_direction::uplink);
  ether::frame_buffer& frame_buffer = scoped_buffer.get_next_frame();
  span<uint8_t>        buffer       = frame_buffer.data();

  // Build the Open Fronthaul control message. Only one port supported.
  units::bytes  ether_hdr_size  = eth_builder->get_header_size();
  units::bytes  ecpri_hdr_size  = ecpri_builder->get_header_size(ecpri::message_type::rt_control_data);
  units::bytes  offset          = ether_hdr_size + ecpri_hdr_size;
  span<uint8_t> ofh_buffer      = buffer.last(buffer.size() - offset.value());
  const auto&   ofh_ctrl_params = generate_prach_control_parameters(context, prach_compr_params, ru_nof_prbs);

  logger.debug("Generated a PRACH request for slot '{}': numSymbols={}, startSym={}, start_re={}, scs={}, "
               "prach_scs={}, nof_rb={}, timeOffset={}, freqOffset={}",
               slot,
               context.nof_repetitions,
               context.start_symbol,
               context.prach_start_re,
               to_string(context.scs),
               to_string(context.prach_scs),
               context.prach_nof_rb,
               ofh_ctrl_params.time_offset,
               ofh_ctrl_params.section_fields.frequency_offset);

  unsigned bytes_written = cp_builder->build_prach_mixed_numerology_message(ofh_buffer, ofh_ctrl_params);
  unsigned eaxc          = context.eaxc;

  ul_cplane_context_repo.add(slot,
                             eaxc,
                             {ofh_ctrl_params.radio_hdr,
                              ofh_ctrl_params.section_fields.common_fields.prb_start,
                              ofh_ctrl_params.section_fields.common_fields.nof_prb,
                              ofh_ctrl_params.section_fields.common_fields.nof_symbols});

  // Add eCPRI header.
  span<uint8_t> ecpri_buffer = buffer.subspan(ether_hdr_size.value(), ecpri_hdr_size.value() + bytes_written);
  unsigned      seq_id       = cp_ul_seq_gen.generate(eaxc);
  ecpri_builder->build_control_packet(ecpri_buffer, generate_ecpri_control_parameters(seq_id, eaxc));

  // Update the number of bytes written.
  bytes_written += ecpri_hdr_size.value();

  // Add Ethernet header.
  span<uint8_t> eth_buffer = span<uint8_t>(buffer).first(ether_hdr_size.value() + bytes_written);
  eth_builder->build_vlan_frame(eth_buffer, vlan_params);

  frame_buffer.set_size(eth_buffer.size());
}
