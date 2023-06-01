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

#include "ofh_uplane_message_builder_impl.h"
#include "../support/network_order_binary_serializer.h"
#include "../support/ofh_uplane_constants.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/compression/iq_compressor.h"
#include "srsran/ran/resource_block.h"

using namespace srsran;
using namespace ofh;

/// Encodes data direction, payload version and filter index.
static uint8_t encode_data_direction(const uplane_message_params& params)
{
  uint8_t octet = 0;

  // Data direction (TX=1); offset: 7, 1 bit long.
  octet |= uint8_t(0x1) << 7u;
  // Payload version; offset: 4, 3 bits long.
  octet |= uint8_t(OFH_PAYLOAD_VERSION) << 4u;
  // Filter index is fixed to 0, skip it.
  return octet;
}

/// Encodes subframe index and MSB bits of slot index.
static uint8_t encode_subframe_and_slot(const uplane_message_params& params)
{
  uint8_t octet      = 0;
  uint8_t slot_index = static_cast<uint8_t>(params.slot.subframe_slot_index());
  // Subframe index; offset: 4, 4 bits long.
  octet |= static_cast<uint8_t>(params.slot.subframe_index()) << 4u;
  // Four MSBs of the slot index within 1ms subframe; offset: 4, 6 bits long.
  octet |= (slot_index >> 2u);
  return octet;
}

/// Encodes remaining LSB bits of the slot index and then symbol index.
static uint8_t encode_slot_lsb_and_symbol(const uplane_message_params& params)
{
  uint8_t octet      = 0;
  uint8_t slot_index = static_cast<uint8_t>(params.slot.subframe_slot_index());
  octet |= (slot_index & uint8_t(0x3)) << 6u;
  octet |= static_cast<uint8_t>(params.symbol_id);
  return octet;
}

/// Encodes and returns the 4 lsb bits section id, the rb bit, number of symbols and the 2 msb bits of start PRB.
static uint8_t encode_sect_id_rb_symbols(const uplane_message_params& params)
{
  uint8_t octet = 0;
  octet |= uint8_t(0) << 3u;
  octet |= uint8_t(0) << 2u;
  octet |= static_cast<uint8_t>(params.start_prb >> 8u) & uint8_t(0x3);
  return octet;
}

/// Writes radio application header to the output buffer.
static void build_radio_app_header(network_order_binary_serializer& serializer, const uplane_message_params& params)
{
  // Data direction + payload version + filter index (1 Byte).
  serializer.write(encode_data_direction(params));

  // Write FrameId (1 Byte) - a counter for 10 ms frames (wrapping period 2.56 seconds), range [0, 256].
  serializer.write(static_cast<uint8_t>(params.slot.sfn() % 256));

  // Write subframe and slot index (1 Byte).
  serializer.write(encode_subframe_and_slot(params));

  // Write 2 LSBs of slot index and symbol index.
  serializer.write(encode_slot_lsb_and_symbol(params));
}

/// Writes section1 header to the output buffer.
static void build_section1_header(network_order_binary_serializer& serializer, const uplane_message_params& params)
{
  // Section ID is fixed to 0.
  serializer.write(uint8_t(0));

  // Write rb, symInc and 2 MSB bits of start PRB.
  serializer.write(encode_sect_id_rb_symbols(params));

  // Write remaining LSBs of start PRB.
  serializer.write(static_cast<uint8_t>(params.start_prb & uint8_t(0xff)));

  // Write number of PRBs.
  serializer.write(static_cast<uint8_t>((params.nof_prb > std::numeric_limits<uint8_t>::max()) ? 0 : params.nof_prb));
}

static void advance_n_prbs(network_order_binary_serializer& serializer,
                           unsigned                         nof_prbs,
                           unsigned                         data_width,
                           compression_type                 comp)
{
  // NRE carriers and IQ data (2 values are written I and Q).
  units::bits prb_bit_size(NRE * 2 * data_width);

  srsran_assert(prb_bit_size.is_byte_exact(), "PRB bit size is not multiple of byte");

  unsigned ud_comp_param_size = (comp == compression_type::none || comp == compression_type::modulation) ? 0 : 1;
  serializer.advance((nof_prbs * (ud_comp_param_size + prb_bit_size.round_up_to_bytes().value())));
}

void uplane_message_builder_impl::serialize_iq_data(network_order_binary_serializer& serializer,
                                                    span<const cf_t>                 symbols,
                                                    unsigned                         nof_prbs,
                                                    const ru_compression_params&     compr_params)
{
  static_vector<compressed_prb, MAX_NOF_PRBS> compressed_prbs(nof_prbs);
  compressor.compress(compressed_prbs, symbols, compr_params);

  // Serialize compression header.
  serialize_compression_header(serializer, compr_params);

  if (ud_comp_length_support) {
    // The udCompLen field shall only be present for the following compression methods:
    // "BFP + selective RE sending" or "Modulation compression + selective RE sending".
    if (compr_params.type == compression_type::bfp_selective || compr_params.type == compression_type::mod_selective) {
      units::bits prb_iq_data_size_bits(NOF_SUBCARRIERS_PER_RB * 2U * compr_params.data_width);
      uint16_t    udCompLen = prb_iq_data_size_bits.round_up_to_bytes().value();
      serializer.write(udCompLen);
    }
  }

  for (compressed_prb& c_prb : compressed_prbs) {
    // Serialize compression parameter.
    if (compr_params.type != compression_type::none && compr_params.type != compression_type::modulation) {
      serializer.write(c_prb.get_compression_param());
    }
    // Serialize compressed data.
    serializer.write(c_prb.get_packed_data());
  }
}

void uplane_message_builder_impl::write_resource_grid(network_order_binary_serializer& serializer,
                                                      const resource_grid_reader&      grid,
                                                      const uplane_message_params&     params)
{
  constexpr unsigned MAX_NOF_SUBCARRIERS = MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB;
  unsigned           data_width          = params.compression_params.data_width;

  // Fill the message with zeros until the grid data is reached.
  if (params.start_prb == 0) {
    // Fill with zeros until the data is reached.
    if (data_start_prb != 0) {
      advance_n_prbs(serializer, data_start_prb, data_width, params.compression_params.type);
      logger.debug("Initial PRB to zero detected. Advancing prb={}:{} , data_width={}", 0, data_start_prb, data_width);
    }

    logger.debug("First nof_prb={} skipped", data_start_prb);

    unsigned nof_prbs_to_read_from_grid = std::min(du_nof_prbs, (params.nof_prb - data_start_prb));

    // Write the grid contents.
    static_vector<cf_t, MAX_NOF_SUBCARRIERS> symbols(nof_prbs_to_read_from_grid * NOF_SUBCARRIERS_PER_RB);
    static_vector<bool, MAX_NOF_SUBCARRIERS> mask(nof_prbs_to_read_from_grid * NOF_SUBCARRIERS_PER_RB, true);
    grid.get(symbols, 0, params.symbol_id, 0, mask);
    serialize_iq_data(serializer, symbols, nof_prbs_to_read_from_grid, params.compression_params);

    logger.debug("Serialized {} PRBs", nof_prbs_to_read_from_grid);

    // Fill the message with zeros when the grid data is already consumed.
    advance_n_prbs(serializer,
                   (params.nof_prb - (data_start_prb + nof_prbs_to_read_from_grid)),
                   data_width,
                   params.compression_params.type);

    return;
  }

  // Second message.
  // No data in the second message.
  if (params.start_prb > data_start_prb + du_nof_prbs) {
    advance_n_prbs(serializer, params.nof_prb, data_width, params.compression_params.type);

    return;
  }

  unsigned nof_prbs_to_read_from_grid = data_start_prb + du_nof_prbs - params.start_prb;
  srsran_assert(nof_prbs_to_read_from_grid > du_nof_prbs,
                "Number of PRBs of the DU has to be greater that the number of messages to write");
  unsigned start_prb = du_nof_prbs - nof_prbs_to_read_from_grid;

  static_vector<cf_t, MAX_RB * NRE> symbols(nof_prbs_to_read_from_grid * NRE);
  static_vector<bool, MAX_RB * NRE> mask(nof_prbs_to_read_from_grid * NRE, true);
  grid.get(symbols, 0, params.symbol_id, start_prb * NRE, mask);
  serialize_iq_data(serializer, symbols, nof_prbs_to_read_from_grid, params.compression_params);

  advance_n_prbs(serializer, (params.nof_prb - nof_prbs_to_read_from_grid), data_width, params.compression_params.type);
}

unsigned uplane_message_builder_impl::build_message(span<uint8_t>                buffer,
                                                    const resource_grid_reader&  grid,
                                                    const uplane_message_params& params)
{
  srsran_assert(params.sect_type == section_type::type_1, "Unsupported section type");

  network_order_binary_serializer serializer(buffer.data());

  build_radio_app_header(serializer, params);
  build_section1_header(serializer, params);
  write_resource_grid(serializer, grid, params);

  return serializer.get_offset();
}

void ofh_uplane_message_builder_static_compression_impl::serialize_compression_header(
    network_order_binary_serializer& serializer,
    const ru_compression_params&     params)
{
  // When the static IQ format is configured the udCompHdr and reserved fields are absent.
}

units::bytes
ofh_uplane_message_builder_static_compression_impl::get_header_size(const ru_compression_params& params) const
{
  units::bytes hdr_size(8);
  if (params.type == compression_type::bfp_selective || params.type == compression_type::mod_selective) {
    hdr_size += units::bytes(2);
  }

  return hdr_size;
}
