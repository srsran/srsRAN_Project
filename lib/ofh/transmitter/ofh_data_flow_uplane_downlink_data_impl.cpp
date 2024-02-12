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

#include "ofh_data_flow_uplane_downlink_data_impl.h"
#include "ofh_uplane_fragment_size_calculator.h"
#include "scoped_frame_buffer.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include <thread>

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
    const data_flow_uplane_downlink_data_impl_config&  config,
    data_flow_uplane_downlink_data_impl_dependencies&& dependencies) :
  logger(*dependencies.logger),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  ru_nof_prbs(config.ru_nof_prbs),
  vlan_params(config.vlan_params),
  compr_params(config.compr_params),
  frame_pool_ptr(dependencies.frame_pool),
  frame_pool(*frame_pool_ptr),
  compressor_sel(std::move(dependencies.compressor_sel)),
  eth_builder(std::move(dependencies.eth_builder)),
  ecpri_builder(std::move(dependencies.ecpri_builder)),
  up_builder(std::move(dependencies.up_builder))
{
  srsran_assert(eth_builder, "Invalid Ethernet VLAN packet builder");
  srsran_assert(ecpri_builder, "Invalid eCPRI packet builder");
  srsran_assert(compressor_sel, "Invalid compressor selector");
  srsran_assert(up_builder, "Invalid User-Plane message builder");
  srsran_assert(frame_pool_ptr, "Invalid frame pool");
}

void data_flow_uplane_downlink_data_impl::enqueue_section_type_1_message(
    const data_flow_uplane_resource_grid_context& context,
    const resource_grid_reader&                   grid)
{
  enqueue_section_type_1_message_symbol_burst(context, grid);
}

void data_flow_uplane_downlink_data_impl::enqueue_section_type_1_message_symbol_burst(
    const data_flow_uplane_resource_grid_context& context,
    const resource_grid_reader&                   grid)
{
  // Temporary buffer used to store IQ data when the RU operating bandwidth is not the same to the cell bandwidth.
  std::array<cf_t, MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB> temp_buffer;
  if (SRSRAN_UNLIKELY(ru_nof_prbs != grid.get_nof_subc())) {
    // Zero out the elements that won't be filled after reading the resource grid.
    std::fill(temp_buffer.begin() + grid.get_nof_subc(), temp_buffer.end(), 0);
  }

  units::bytes headers_size = eth_builder->get_header_size() +
                              ecpri_builder->get_header_size(ecpri::message_type::iq_data) +
                              up_builder->get_header_size(compr_params);

  // Iterate over all the symbols.
  for (unsigned symbol_id = context.symbol_range.start(), symbol_end = context.symbol_range.length();
       symbol_id != symbol_end;
       ++symbol_id) {
    slot_symbol_point   symbol_point(context.slot, symbol_id, nof_symbols_per_slot);
    scoped_frame_buffer scoped_buffer(frame_pool, symbol_point, message_type::user_plane, data_direction::downlink);
    if (scoped_buffer.empty()) {
      logger.warning("Not enough space in the buffer pool to create a downlink User-Plane message for slot '{}' and "
                     "eAxC '{}', symbol_id '{}'",
                     context.slot,
                     context.eaxc,
                     symbol_id);
      return;
    }

    span<const cf_t> iq_data;
    if (SRSRAN_LIKELY(ru_nof_prbs == grid.get_nof_subc())) {
      iq_data = grid.get_view(context.port, symbol_id);
    } else {
      span<cf_t> temp_iq_data(temp_buffer.data(), ru_nof_prbs * NOF_SUBCARRIERS_PER_RB);
      grid.get(temp_iq_data.first(grid.get_nof_subc()), context.port, symbol_id, 0);
      iq_data = temp_iq_data;
    }

    // Split the data into multiple messages when it does not fit into a single one.
    ofh_uplane_fragment_size_calculator prb_fragment_calculator(0, ru_nof_prbs, compr_params);
    bool                                is_last_fragment   = false;
    unsigned                            fragment_start_prb = 0U;
    unsigned                            fragment_nof_prbs  = 0U;
    do {
      ether::frame_buffer& frame_buffer = scoped_buffer.get_next_frame();
      span<uint8_t>        data         = frame_buffer.data();

      is_last_fragment = prb_fragment_calculator.calculate_fragment_size(
          fragment_start_prb, fragment_nof_prbs, data.size() - headers_size.value());

      // Skip frame buffers so small that cannot carry one PRB.
      if (fragment_nof_prbs == 0) {
        logger.warning(
            "Skipped frame buffer as it cannot store data for a single PRB, required buffer size is '{}' bytes",
            data.size());

        continue;
      }

      const uplane_message_params& up_params =
          generate_dl_ofh_user_parameters(context.slot, symbol_id, fragment_start_prb, fragment_nof_prbs, compr_params);

      unsigned used_size = enqueue_section_type_1_message_symbol(
          iq_data.subspan(fragment_start_prb * NOF_SUBCARRIERS_PER_RB, fragment_nof_prbs * NOF_SUBCARRIERS_PER_RB),
          up_params,
          context.eaxc,
          data);
      frame_buffer.set_size(used_size);
    } while (!is_last_fragment);
  }
}

unsigned data_flow_uplane_downlink_data_impl::enqueue_section_type_1_message_symbol(span<const cf_t> iq_symbol_data,
                                                                                    const uplane_message_params& params,
                                                                                    unsigned                     eaxc,
                                                                                    span<uint8_t>                buffer)
{
  // Build the Open Fronthaul data message. Only one port supported.
  units::bytes  ether_header_size = eth_builder->get_header_size();
  units::bytes  ecpri_hdr_size    = ecpri_builder->get_header_size(ecpri::message_type::iq_data);
  units::bytes  offset            = ether_header_size + ecpri_hdr_size;
  span<uint8_t> ofh_buffer        = span<uint8_t>(buffer).last(buffer.size() - offset.value());
  unsigned      bytes_written     = up_builder->build_message(ofh_buffer, iq_symbol_data, params);

  // Add eCPRI header. Create a subspan with the payload that skips the Ethernet header.
  span<uint8_t> ecpri_buffer =
      span<uint8_t>(buffer).subspan(ether_header_size.value(), ecpri_hdr_size.value() + bytes_written);
  ecpri_builder->build_data_packet(ecpri_buffer, generate_ecpri_data_parameters(up_seq_gen.generate(eaxc), eaxc));

  // Update the number of bytes written.
  bytes_written += ecpri_hdr_size.value();

  // Add Ethernet header.
  span<uint8_t> eth_buffer = span<uint8_t>(buffer).first(ether_header_size.value() + bytes_written);
  eth_builder->build_vlan_frame(eth_buffer, vlan_params);

  logger.debug("Packing a downlink User-Plane message for slot '{}' and eAxC '{}', symbol_id '{}', PRB range '{}:{}', "
               "size '{}' bytes",
               params.slot,
               eaxc,
               params.symbol_id,
               params.start_prb,
               params.nof_prb,
               eth_buffer.size());

  return eth_buffer.size();
}
