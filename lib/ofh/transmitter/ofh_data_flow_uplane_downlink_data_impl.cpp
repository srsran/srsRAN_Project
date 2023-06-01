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

#include "ofh_data_flow_uplane_downlink_data_impl.h"
#include "ofh_uplane_fragment_size_calculator.h"
#include "scoped_frame_buffer.h"
#include "srsran/phy/support/resource_grid_context.h"

using namespace srsran;
using namespace ofh;

/// Generates and returns the downlink Open Fronthaul user parameters for the given context.
static uplane_message_params generate_dl_ofh_user_parameters(slot_point                   slot,
                                                             unsigned                     symbol_id,
                                                             unsigned                     start_prb,
                                                             unsigned                     nof_prb,
                                                             const ru_compression_params& comp)
{
  uplane_message_params params;
  params.direction                     = data_direction::downlink;
  params.payload_version               = 1;
  params.slot                          = slot;
  params.filter_index                  = srsran::ofh::filter_index_type::standard_channel_filter;
  params.start_prb                     = start_prb;
  params.nof_prb                       = nof_prb;
  params.symbol_id                     = symbol_id;
  params.sect_type                     = section_type::type_1;
  params.compression_params.type       = comp.type;
  params.compression_params.data_width = comp.data_width;

  return params;
}

/// Generates and returns the eCPRI IQ data parameters.
static ecpri::iq_data_parameters generate_ecpri_data_parameters(uint16_t seq_id, uint16_t eaxc)
{
  ecpri::iq_data_parameters params;
  // Only supporting 1 Port, 1 band and 1 CC.
  params.pc_id  = eaxc;
  params.seq_id = 0;

  // Set seq_id.
  params.seq_id |= (seq_id & 0x00ff) << 8;
  // Set the E bit to 1 and subsequence ID to 0. E bit set to 1 indicates that there is no radio transport layer
  // fragmentation.
  params.seq_id |= uint16_t(1U) << 7;

  return params;
}

data_flow_uplane_downlink_data_impl::data_flow_uplane_downlink_data_impl(
    data_flow_uplane_downlink_data_impl_config&& config) :
  nof_symbols(config.nof_symbols),
  ru_nof_prbs(config.ru_nof_prbs),
  vlan_params(config.vlan_params),
  compr_params(config.compr_params),
  logger(*config.logger),
  frame_pool_ptr(config.frame_pool),
  frame_pool(*frame_pool_ptr),
  compressor_sel(std::move(config.compressor_sel)),
  eth_builder(std::move(config.eth_builder)),
  ecpri_builder(std::move(config.ecpri_builder)),
  up_builder(std::move(config.up_builder))
{
  srsran_assert(eth_builder, "Invalid Ethernet VLAN packet builder");
  srsran_assert(ecpri_builder, "Invalid eCPRI packet builder");
  srsran_assert(compressor_sel, "Invalid compressor selector");
  srsran_assert(up_builder, "Invalid User-Plane message builder");
}

void data_flow_uplane_downlink_data_impl::enqueue_section_type_1_message(const resource_grid_context& context,
                                                                         const resource_grid_reader&  grid,
                                                                         unsigned                     eaxc)
{
  enqueue_section_type_1_message_symbol_burst(context, grid, eaxc);
}

void data_flow_uplane_downlink_data_impl::enqueue_section_type_1_message_symbol_burst(
    const resource_grid_context& context,
    const resource_grid_reader&  grid,
    unsigned                     eaxc)
{
  units::bytes headers_size = eth_builder->get_header_size() +
                              ecpri_builder->get_header_size(ecpri::message_type::iq_data) +
                              up_builder->get_header_size(compr_params);

  // Iterate over all the symbols.
  for (unsigned symbol_id = 0, symbol_end = nof_symbols; symbol_id != symbol_end; ++symbol_id) {
    scoped_frame_buffer                 scoped_buffer(frame_pool, context.slot, symbol_id, message_type::user_plane);
    ofh_uplane_fragment_size_calculator prb_fragment_calculator(0, ru_nof_prbs, compr_params);
    // Split the data into multiple messages when it does not fit into a single one.
    bool     is_last_fragment   = false;
    unsigned fragment_start_prb = 0U;
    unsigned fragment_nof_prbs  = 0U;

    do {
      ether::frame_buffer& frame_buffer = scoped_buffer.get_next_frame();
      span<uint8_t>        data         = frame_buffer.data();

      is_last_fragment = prb_fragment_calculator.calculate_fragment_size(
          fragment_start_prb, fragment_nof_prbs, data.size() - headers_size.value());

      // Skip frame buffers so small that cannot carry one PRB.
      if (fragment_nof_prbs == 0) {
        logger.warning("Skipping frame buffer as it cannot carry a PRB. Frame buffer size={}", data.size());

        continue;
      }

      unsigned used_size = enqueue_section_type_1_message_symbol(
          context, grid, symbol_id, fragment_start_prb, fragment_nof_prbs, eaxc, data);
      frame_buffer.set_size(used_size);
    } while (!is_last_fragment);
  }
}

unsigned
data_flow_uplane_downlink_data_impl::enqueue_section_type_1_message_symbol(const resource_grid_context& context,
                                                                           const resource_grid_reader&  grid,
                                                                           unsigned                     symbol_id,
                                                                           unsigned                     start_prb,
                                                                           unsigned                     nof_prb,
                                                                           unsigned                     eaxc,
                                                                           span<uint8_t>                buffer)
{
  // Build the Open Fronthaul data message. Only one port supported.
  units::bytes  ether_header_size = eth_builder->get_header_size();
  units::bytes  ecpri_hdr_size    = ecpri_builder->get_header_size(ecpri::message_type::iq_data);
  units::bytes  offset            = ether_header_size + ecpri_hdr_size;
  span<uint8_t> ofh_buffer        = span<uint8_t>(buffer).last(buffer.size() - offset.value());
  unsigned      bytes_written     = up_builder->build_message(
      ofh_buffer, grid, generate_dl_ofh_user_parameters(context.slot, symbol_id, start_prb, nof_prb, compr_params));

  // Add eCPRI header. Create a subspan with the payload that skips the Ethernet header.
  span<uint8_t> ecpri_buffer =
      span<uint8_t>(buffer).subspan(ether_header_size.value(), ecpri_hdr_size.value() + bytes_written);
  ecpri_builder->build_data_packet(ecpri_buffer, generate_ecpri_data_parameters(up_seq_gen.generate(eaxc), eaxc));

  // Update the number of bytes written.
  bytes_written += ecpri_hdr_size.value();

  // Add Ethernet header.
  span<uint8_t> eth_buffer = span<uint8_t>(buffer).first(ether_header_size.value() + bytes_written);
  eth_builder->build_vlan_frame(eth_buffer, vlan_params);

  logger.debug("Creating User-Plane message for downlink at slot={}, symbol_id={}, prbs={}:{}, size={}",
               context.slot,
               symbol_id,
               start_prb,
               nof_prb,
               eth_buffer.size());

  return eth_buffer.size();
}
